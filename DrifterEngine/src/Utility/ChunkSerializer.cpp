#include "pch.h"
#include "ChunkSerializer.h"
#include <Utility/CompressData.h>
#include <Snapshot/EnTTSnapshot.h>
#include <Engine/CommonEngineDirectories.h>
#include <Utility/StandardLogger.h>

const std::filesystem::path CHUNK_LIST_FILEPATH = REGION_DIRECTORY / "chunk_list.dat";
static const size_t PROMISE_LIST_RESERVE = 100u;

drft::ChunkSerializer::ChunkSerializer()
{
	std::filesystem::create_directory(REGION_DIRECTORY);
	loadSerializedChunkList();
	_shouldShutdown = false;
	_serializationThread = std::thread(&ChunkSerializer::serializationThread, this);
}

drft::ChunkSerializer::~ChunkSerializer()
{
	_shouldShutdown = true;
	_serializationThread.join();
	saveSerializedChunkList();
}

bool drft::ChunkSerializer::isSerialized(SourceChunkPositionPair position) const
{
	return _serializedChunks.contains(position);
}

std::future<entt::registry> drft::ChunkSerializer::queueForSave(SourceChunkPositionPair position, entt::registry&& registry)
{
	{
		std::lock_guard<std::mutex> lock(_saveQueueLock);
		_saveQueue.emplace_back(position, std::move(registry));
	}

	_serializedChunks.insert(position); // HACKZ: Maybe not the best place to add because it assumes serialization worked, but avoids needing a mutex
	
	std::lock_guard<std::mutex> lock(_savePromiseLock);
	_savePromises.emplace(position, std::promise<entt::registry>{});
	return _savePromises.at(position).get_future();
}

std::future<entt::registry> drft::ChunkSerializer::queueForLoad(SourceChunkPositionPair position, entt::registry&& registry)
{
	{
		std::lock_guard<std::mutex> lock(_loadQueueLock);
		_loadQueue.emplace_back(position, std::move(registry));
	}

	std::lock_guard<std::mutex> lock(_loadPromiseLock);
	_loadPromises.emplace(position, std::promise<entt::registry>{});
	return _loadPromises.at(position).get_future();
}

std::vector<char> drft::ChunkSerializer::serializeAndCompressRegistry(const entt::registry& registry)
{
	std::stringstream oss;
	cereal::BinaryOutputArchive output{ oss };
	snapshot::Snapshot::save(output, registry);

	std::vector<char> data;
	data.assign(std::istreambuf_iterator<char>(oss), std::istreambuf_iterator<char>());

	return util::compressData(data);
}

void drft::ChunkSerializer::decompressAndDeserializeRegistry(const std::vector<char>& compressed, entt::registry& registry)
{
	auto decompressed = util::decompressData(compressed);
	std::stringstream iss;
	iss.write(decompressed.data(), decompressed.size());
	cereal::BinaryInputArchive input{ iss };
	snapshot::SnapshotLoader::load(input, registry);
}

void drft::ChunkSerializer::serializationThread()
{
	do 
	{
		try {
			processSaveList();
			processLoadList();

			syncSaveList();
			syncLoadList();
		}
		catch (std::exception e)
		{
			LOG_ERROR("Error in serialization thread: {}", e.what());
		}
	} 
	while (!_shouldShutdown || !_saveList.empty() || !_loadList.empty());
}

void drft::ChunkSerializer::syncSaveList()
{
	std::lock_guard<std::mutex> lock(_saveQueueLock);
	for (auto&& [position, registry] : _saveQueue)
	{
		auto path = getRegionFilePath(position);
		_saveList[path].emplace_back(position, std::move(registry));
	}
	_saveQueue.clear();
}

void drft::ChunkSerializer::syncLoadList()
{
	std::lock_guard<std::mutex> lock(_loadQueueLock);
	for (auto&& [position, registry] : _loadQueue)
	{
		auto path = getRegionFilePath(position);
		_loadList[path].emplace_back(position, std::move(registry));
	}
	_loadQueue.clear();
}

void drft::ChunkSerializer::processSaveList()
{
	std::vector<SourceChunkPositionPair> promisesToComplete;
	promisesToComplete.reserve(PROMISE_LIST_RESERVE);
	for (auto&& [filepath, list] : _saveList)
	{
		if (!_regionFiles.contains(filepath))
		{
			_regionFiles.emplace(filepath, REGION_DIRECTORY / filepath);
		}

		RegionFile& regionFile = _regionFiles.at(filepath);
		if (regionFile.open())
		{
			for (auto&& [sourcePositionPair, registry] : list)
			{
				const CompressedChunk compressed = serializeAndCompressRegistry(registry);
				regionFile.writeChunk(sourcePositionPair.position, compressed);
				promisesToComplete.push_back(sourcePositionPair);
			}
		}
		regionFile.close();
	}

	_saveList.clear();

	std::lock_guard<std::mutex> lock(_savePromiseLock);
	for (auto&& chunk : promisesToComplete)
	{
		_savePromises.at(chunk).set_value({}); // The registry won't be needed by the requester
		_savePromises.erase(chunk);
	}
}
 
void drft::ChunkSerializer::processLoadList()
{
	std::vector<SourceChunkPositionPair> promisesToComplete;
	promisesToComplete.reserve(PROMISE_LIST_RESERVE);
	for (auto&& [filepath, list] : _loadList)
	{
		if (!_regionFiles.contains(filepath))
		{
			_regionFiles.emplace(filepath, REGION_DIRECTORY / filepath);
		}

		RegionFile& regionFile = _regionFiles.at(filepath);
		if (regionFile.open())
		{
			for (auto&& [sourcePositionPair, registry] : list)
			{
				auto compressed = regionFile.readChunk(sourcePositionPair.position);
				decompressAndDeserializeRegistry(compressed, registry);

				std::lock_guard<std::mutex> lock(_loadPromiseLock);
				_loadPromises.at(sourcePositionPair).set_value(std::move(registry));
				_loadPromises.erase(sourcePositionPair);
			}
		}
		regionFile.close();
	}

	_loadList.clear();
}

void drft::ChunkSerializer::saveSerializedChunkList()
{
	std::ofstream file{ CHUNK_LIST_FILEPATH, std::ios::trunc | std::ios::binary };
	cereal::BinaryOutputArchive archive{ file };
	archive(_serializedChunks);
}

void drft::ChunkSerializer::loadSerializedChunkList()
{
	if (!std::filesystem::exists(CHUNK_LIST_FILEPATH)) return;

	std::ifstream file{ CHUNK_LIST_FILEPATH, std::ios::binary };
	cereal::BinaryInputArchive archive{ file };
	archive(_serializedChunks);
}

std::filesystem::path drft::ChunkSerializer::getRegionFilePath(SourceChunkPositionPair sourcePositionPair) const
{
	const std::string filename = std::format(
		"r_{}_{}_{}.dat",
		sourcePositionPair.position.x / REGION_DIMENSIONS,
		sourcePositionPair.position.y / REGION_DIMENSIONS,
		sourcePositionPair.position.z);

	return std::filesystem::path{ std::to_string(sourcePositionPair.sourceId) } / filename;
}
