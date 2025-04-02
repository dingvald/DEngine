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

	_isFlushed = true;
	_isFlushed &= processBuildQueue(registry);
	_isFlushed &= processLoadQueue(registry);
	_isFlushed &= processSaveQueue(registry);

	cleanUpSavedChunks(registry);
}

void drft::spatial::ChunkSource::shutdown(entt::registry& registry)
{
	for (auto&& [coord, chunk] : _chunks)
	{
		_toSave.push_back(coord);
		chunk.setState(spatial::ChunkState::ToSave);
	}
	
	processSaveQueue(registry);
	
	cleanUpAllChunks(registry);
}

bool drft::spatial::ChunkSource::isReady() const
{
	return true;
}

entt::id_type drft::spatial::ChunkSource::id() const
{
	return _sourceId;
}

void drft::spatial::ChunkSource::updateChunkStates(TilePosition cameraPosition)
{
	if (_isShuttingDown) return;

	ChunkPosition cameraChunkPosition = spatial::toChunkSpace(cameraPosition);

	auto activeCoords = spatial::getIntCircleInRadius(cameraChunkPosition, ACTIVE_CHUNK_RADIUS_XY);

	// Ensure active chunks are active or will be built
	for (auto&& coord : activeCoords)
	{
		const ChunkPosition chunkPosition = spatial::asChunkSpace(coord);
		if (!_chunks.contains(chunkPosition))
		{
			_chunks.emplace(chunkPosition, spatial::VirtualChunk{_sourceId, chunkPosition });
		}

		spatial::VirtualChunk& chunk = _chunks.at(chunkPosition);
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
	for (auto&& [coord, chunk] : _chunks)
	{
		if (chunk.getState() != spatial::ChunkState::Active) continue;

		if (isWithinChunkSaveArea(coord, cameraChunkPosition)) continue;

		_toSave.push_back(coord);
		chunk.setState(spatial::ChunkState::ToSave);
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

void drft::spatial::ChunkSource::flushAllQueues(entt::registry& registry)
{
}

bool drft::spatial::ChunkSource::processBuildQueue(entt::registry& registry)
{
	if (_toBuild.empty()) return true;

	bool anyPending = false;
	std::vector<ChunkPosition> toRemove;
	for (auto&& coord : _toBuild)
	{
		spatial::VirtualChunk& chunk = _chunks.at(coord);

		spatial::ioStatus status = chunk.build(registry, _generator);
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

bool drft::spatial::ChunkSource::isWithinChunkSaveArea(sf::Vector3i chunkPosition, sf::Vector3i centerPosition) const
{
	const int dz_abs = std::abs(chunkPosition.z - centerPosition.z);
	if (dz_abs == 0)
	{
		return spatial::isWithinRadius2d({ centerPosition.x, centerPosition.y }, { chunkPosition.x, chunkPosition.y }, TO_SAVE_CHUNK_RADIUS_XY);
	}
	return false;
}
