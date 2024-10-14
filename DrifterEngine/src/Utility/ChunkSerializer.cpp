#include "pch.h"
#include "ChunkSerializer.h"
#include <Utility/VectorBuffer.h>
#include <Utility/CompressData.h>
#include <Snapshot/EnTTSnapshot.h>
#include <Engine/CommonEngineDirectories.h>
#include <Utility/StandardLogger.h>

drft::ChunkSerializer::ChunkSerializer()
{
	std::filesystem::create_directory(REGION_DIRECTORY);
}

std::future<void> drft::ChunkSerializer::queueForSave(ChunkPosition position, entt::registry& registry)
{
	{
		std::lock_guard<std::mutex> lock(_saveQueueLock);
		_saveQueue.emplace_back(position, registry);
	}
	
	std::lock_guard<std::mutex> lock(_savePromiseLock);
	_savePromises.emplace(position, std::promise<void>{});
	return _savePromises.at(position).get_future();
}

std::future<void> drft::ChunkSerializer::queueForLoad(ChunkPosition position, entt::registry& registry)
{
	{
		std::lock_guard<std::mutex> lock(_loadQueueLock);
		_loadQueue.emplace_back(position, registry);
	}

	std::lock_guard<std::mutex> lock(_loadPromiseLock);
	_loadPromises.emplace(position, std::promise<void>{});
	return _loadPromises.at(position).get_future();
}

void drft::ChunkSerializer::serializationThread()
{
	syncSaveList();
	syncLoadList();

	processSaveList();
	processLoadList();
}

void drft::ChunkSerializer::syncSaveList()
{
	std::lock_guard<std::mutex> lock(_saveQueueLock);
	for (auto&& [position, registry] : _saveQueue)
	{
		auto path = getRegionFilePath(position);
		_saveList[path].emplace_back(position, registry);
	}
}

void drft::ChunkSerializer::syncLoadList()
{
	std::lock_guard<std::mutex> lock(_loadQueueLock);
	for (auto&& [position, registry] : _loadQueue)
	{
		auto path = getRegionFilePath(position);
		_loadList[path].emplace_back(position, registry);
	}
}

void drft::ChunkSerializer::processSaveList()
{
	std::vector<ChunkPosition> promisesToComplete;
	for (auto&& [region, list] : _saveList)
	{
		if (!_regionFiles.contains(region))
		{
			_regionFiles.emplace(region, region);
		}

		RegionFile& regionFile = _regionFiles.at(region);
		if (regionFile.open())
		{
			for (auto&& [position, registry] : list)
			{
				auto compressed = serializeAndCompressChunk(registry);
				regionFile.writeChunk(position, compressed);
				promisesToComplete.push_back(position);
			}
		}
		regionFile.close();
	}

	std::lock_guard<std::mutex> lock(_savePromiseLock);
	for (auto&& chunk : promisesToComplete)
	{
		_savePromises.at(chunk).set_value();
		_savePromises.erase(chunk);
	}
}

void drft::ChunkSerializer::processLoadList()
{
}

std::vector<char> drft::ChunkSerializer::serializeAndCompressChunk(entt::registry& registry) const
{
	VectorBuffer buffer;
	std::ostream os{ &buffer };
	cereal::BinaryOutputArchive output{ os };
	snapshot::Snapshot::save(output, registry);

	return util::compressData(buffer);
}

std::filesystem::path drft::ChunkSerializer::getRegionFilePath(ChunkPosition position) const
{
	std::string filename = "r_"
		+ std::to_string(position.x / REGION_DIMENSIONS)
		+ "_"
		+ std::to_string(position.y / REGION_DIMENSIONS)
		+ "_"
		+ std::to_string(position.z)
		+ ".dat";
	return REGION_DIRECTORY / filename;
}
