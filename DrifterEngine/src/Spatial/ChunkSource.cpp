#include "pch.h"
#include "ChunkSource.h"

#include <Spatial/Helpers.h>
#include <Spatial/Conversions.h>
#include <Spatial/WorldGrid.h>
#include <ProcGen/IChunkGenerator.h>

static constexpr int ACTIVE_CHUNK_RADIUS_XY = 10;
static constexpr int TO_SAVE_CHUNK_RADIUS_XY = ACTIVE_CHUNK_RADIUS_XY + 2;

drft::spatial::ChunkSource::ChunkSource(entt::id_type sourceId, ChunkSerializer& serializer, IChunkGenerator& generator)
	: _sourceId(sourceId)
	, _serializer(serializer)
	, _generator(generator)
{
}

void drft::spatial::ChunkSource::update(TilePosition cameraPosition, entt::registry& registry)
{
	updateChunkStates(cameraPosition);

	processBuildQueue(registry);
	processLoadQueue(registry);
	processSaveQueue(registry);

	cleanUpSavedChunks(registry);
}

void drft::spatial::ChunkSource::shutdown(entt::registry& registry, bool isAsync)
{
	_isShuttingDown = true;
	for (auto&& [coord, chunk] : _chunks)
	{
		_toSave.push_back(coord);
		chunk.setState(spatial::ChunkState::ToSave);
	}

	bool isDone = false;
	do
	{
		isDone = processSaveQueue(registry);
	} 
	while (!isDone && !isAsync);
	
	cleanUpAllChunks(registry);
}

void drft::spatial::ChunkSource::setBuildsPerFrame(unsigned int value)
{
	_buildsPerFrame = value;
}

bool drft::spatial::ChunkSource::isLoadedAroundPosition(TilePosition position) const
{
	ChunkPosition chunkPosition = spatial::toChunkSpace(position);
	auto activeCoords = spatial::getIntCircleInRadius(chunkPosition, ACTIVE_CHUNK_RADIUS_XY);
	for (auto&& coord : activeCoords)
	{
		const ChunkPosition chunkPosition = spatial::asChunkSpace(coord);
		if (!_chunks.contains(chunkPosition)) return false;
		if (_chunks.at(chunkPosition).getState() != spatial::ChunkState::Active) return false;
	}

	return true;
}

entt::id_type drft::spatial::ChunkSource::id() const
{
	return _sourceId;
}

void drft::spatial::ChunkSource::updateChunkStates(TilePosition position)
{
	if (_isShuttingDown) return;

	ChunkPosition centerChunkPosition = spatial::toChunkSpace(position);

	auto activeCoords = spatial::getIntCircleInRadius(centerChunkPosition, ACTIVE_CHUNK_RADIUS_XY);

	// Ensure active chunks are active or will be built
	for (auto&& coord : activeCoords)
	{
		const ChunkPosition chunkPosition = spatial::asChunkSpace(coord);
		auto&& [mapKeyValue, inserted] = _chunks.try_emplace(chunkPosition, spatial::VirtualChunk{ _sourceId, chunkPosition });

		spatial::VirtualChunk& chunk = mapKeyValue->second;
		switch (chunk.getState())
		{
		case spatial::ChunkState::None:
			loadOrBuildChunk(chunkPosition, chunk);
			break;
		case spatial::ChunkState::Built:
			chunk.setState(spatial::ChunkState::Active);
			break;
		case spatial::ChunkState::Loaded:
			chunk.setState(spatial::ChunkState::Active);
			break;
		case spatial::ChunkState::Saved:
			loadOrBuildChunk(chunkPosition, chunk);
			break;
		default:
			break;
		}
	}

	// Then, scan for chunks to save
	for (auto&& [chunkPosition, chunk] : _chunks)
	{
		if (chunk.getState() != spatial::ChunkState::Active) continue;

		if (isWithinActiveArea(chunkPosition, centerChunkPosition)) continue;

		saveChunk(chunkPosition, chunk);
	}
}

void drft::spatial::ChunkSource::cleanUpSavedChunks(entt::registry& registry)
{
	auto& grid = registry.ctx().get<spatial::WorldGrid&>();
	for (auto&& chunkPosition : _toDelete)
	{
		grid.removeChunk(chunkPosition);
		_chunks.erase(chunkPosition);
	}
	_toDelete.clear();
}

void drft::spatial::ChunkSource::cleanUpAllChunks(entt::registry& registry)
{
	auto& grid = registry.ctx().get<spatial::WorldGrid&>();
	for (auto&& [position, chunk] : _chunks)
	{
		grid.removeChunk(position);
	}
	_chunks.clear();
}

bool drft::spatial::ChunkSource::processBuildQueue(entt::registry& registry)
{
	if (_toBuild.empty()) return true;

	bool anyPending = false;
	unsigned int kickedOffBuilds = 0;
	std::vector<ChunkPosition> toRemove;
	for (auto&& coord : _toBuild)
	{
		kickedOffBuilds++;
		if (kickedOffBuilds > _buildsPerFrame) break;

		spatial::ioStatus status = _chunks.at(coord).build(registry, _generator);
		if (status == spatial::ioStatus::Done)
		{
			toRemove.push_back(coord);
			continue;
		}
		anyPending = true;
	}
	for (auto&& coord : toRemove)
	{
		std::erase(_toBuild, coord);
	}
	return !anyPending;
}

bool drft::spatial::ChunkSource::processLoadQueue(entt::registry& registry)
{
	if (_toLoad.empty()) return true;

	bool anyPending = false;
	std::vector<ChunkPosition> toRemove;
	for (auto&& coord : _toLoad)
	{
		spatial::VirtualChunk& chunk = _chunks.at(coord);

		spatial::ioStatus status = chunk.asyncLoad(registry, _serializer);
		if (status == spatial::ioStatus::Done)
		{
			toRemove.push_back(coord);
			continue;
		}
		anyPending = true;
	}
	for (auto&& coord : toRemove)
	{
		std::erase(_toLoad, coord);
	}
	return !anyPending;
}

bool drft::spatial::ChunkSource::processSaveQueue(entt::registry& registry)
{
	if (_toSave.empty()) return true;

	bool anyPending = false;
	std::vector<ChunkPosition> toRemove;
	for (auto&& coord : _toSave)
	{
		spatial::VirtualChunk& chunk = _chunks.at(coord);

		spatial::ioStatus status = chunk.asyncSave(registry, _serializer);
		if (status == spatial::ioStatus::Done)
		{
			toRemove.push_back(coord);
			continue;
		}
		anyPending = true;
	}
	for (auto&& coord : toRemove)
	{
		std::erase(_toSave, coord);
		_toDelete.push_back(coord);
	}
	return !anyPending;
}

void drft::spatial::ChunkSource::loadOrBuildChunk(ChunkPosition position, spatial::VirtualChunk& chunk)
{
	if (_serializer.isSerialized({ _sourceId, position }))
	{
		chunk.setState(spatial::ChunkState::ToLoad);
		_toLoad.push_back(std::move(position));
	}
	else
	{
		chunk.setState(spatial::ChunkState::ToBuild);
		_toBuild.push_back(std::move(position));
	}
}

void drft::spatial::ChunkSource::saveChunk(ChunkPosition position, spatial::VirtualChunk& chunk)
{
	chunk.setState(spatial::ChunkState::ToSave);
	_toSave.push_back(position);
}

bool drft::spatial::ChunkSource::isWithinActiveArea(sf::Vector3i chunkPosition, sf::Vector3i centerPosition) const
{
	const int dz_abs = std::abs(chunkPosition.z - centerPosition.z);
	if (dz_abs == 0)
	{
		return spatial::isWithinRadius2d({ centerPosition.x, centerPosition.y }, { chunkPosition.x, chunkPosition.y }, TO_SAVE_CHUNK_RADIUS_XY);
	}
	return false;
}
