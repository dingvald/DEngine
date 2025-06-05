#include "pch.h"
#include "ChunkSerializer.h"
#include <Utility/VectorBuffer.h>
#include <Utility/CompressData.h>
#include <Snapshot/EnTTSnapshot.h>
#include <Engine/CommonEngineDirectories.h>
#include <Utility/StandardLogger.h>

const std::filesystem::path CHUNK_LIST_FILEPATH = REGION_DIRECTORY / "chunk_list.dat";

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

std::future<void> drft::ChunkSerializer::queueForSave(SourceChunkPositionPair position, entt::registry& registry)
{
	{
		std::lock_guard<std::mutex> lock(_saveQueueLock);
		_saveQueue.emplace_back(position, registry);
	}

	_serializedChunks.insert(position); // HACKZ: Maybe not the best place to add because it assumes serialization worked, but avoids needing a mutex
	
	std::lock_guard<std::mutex> lock(_savePromiseLock);
	_savePromises.emplace(position, std::promise<void>{});
	return _savePromises.at(position).get_future();
}

std::future<void> drft::ChunkSerializer::queueForLoad(SourceChunkPositionPair position, entt::registry& registry)
{
	{
		std::lock_guard<std::mutex> lock(_loadQueueLock);
		_loadQueue.emplace_back(position, registry);
	}

	std::lock_guard<std::mutex> lock(_loadPromiseLock);
	_loadPromises.emplace(position, std::promise<void>{});
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
			error_logger << "Error in serlialization thread: " << e.what() << std::endl;
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
		_saveList[path].emplace_back(position, registry);
	}
	_saveQueue.clear();
}

void drft::ChunkSerializer::syncLoadList()
{
	std::lock_guard<std::mutex> lock(_loadQueueLock);
	for (auto&& [position, registry] : _loadQueue)
	{
		auto path = getRegionFilePath(position);
		_loadList[path].emplace_back(position, registry);
	}
	_loadQueue.clear();
}

void drft::ChunkSerializer::processSaveList()
{
	std::vector<SourceChunkPositionPair> promisesToComplete;
	for (auto&& [region, list] : _saveList)
	{
		if (!_regionFiles.contains(region))
		{
			_regionFiles.emplace(region, region);
		}

		RegionFile& regionFile = _regionFiles.at(region);
		if (regionFile.open())
		{
			for (auto&& [sourcePositionPair, registry] : list)
			{
				auto compressed = serializeAndCompressRegistry(registry);
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
		_savePromises.at(chunk).set_value();
		_savePromises.erase(chunk);
	}
}

void drft::ChunkSerializer::processLoadList()
{
	for (auto&& [region, list] : _loadList)
	{
		std::vector<SourceChunkPositionPair> promisesToComplete;
		promisesToComplete.reserve(list.size());
		if (!_regionFiles.contains(region))
		{
			_regionFiles.emplace(region, region);
		}

		RegionFile& regionFile = _regionFiles.at(region);
		if (regionFile.open())
		{
			for (auto&& [sourcePositionPair, registry] : list)
			{
				auto compressed = regionFile.readChunk(sourcePositionPair.position);
				decompressAndDeserializeRegistry(compressed, registry);
				promisesToComplete.push_back(sourcePositionPair);
			}
		}
		regionFile.close();

		std::lock_guard<std::mutex> lock(_loadPromiseLock);
		for (auto&& chunk : promisesToComplete)
		{
			_loadPromises.at(chunk).set_value();
			_loadPromises.erase(chunk);
		}
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
	std::string filename = "r_"
		+ std::to_string(sourcePositionPair.position.x / REGION_DIMENSIONS)
		+ "_"
		+ std::to_string(sourcePositionPair.position.y / REGION_DIMENSIONS)
		+ "_"
		+ std::to_string(sourcePositionPair.position.z)
		+ ".dat";
	return REGION_DIRECTORY / std::to_string(sourcePositionPair.sourceId) / filename;
}
