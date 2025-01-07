#include "pch.h"
#include "ChunkManager.h"
#include <Engine/CommonEngineDirectories.h>
#include "Spatial/WorldGrid.h"
#include "Spatial/Conversions.h"
#include "Spatial/Helpers.h"
#include "Components/CameraComponent.h"
#include "Components/PositionComponent.h"
#include "ProcGen/WorldGeneration/WorldGenerator.h"

#include "Services/DebugInfo.h"
#include "Systems/Helpers/GetCurrentCamera.h"


static constexpr int ACTIVE_CHUNK_RADIUS_XY = 10;
static constexpr int TO_SAVE_CHUNK_RADIUS_XY = ACTIVE_CHUNK_RADIUS_XY + 2;

void drft::system::ChunkManager::update()
{
	const CameraInfo camera = getCurrentCamera(_registry);
	if (!camera.isInitialized) return;

	updateChunkStates(camera);

	processBuildQueue();
	processLoadQueue();
	processSaveQueue();

	cleanUpChunks();

	service::DebugInfo::instance().putInfo("Active chunks", std::to_string(_chunks.size()));
	service::DebugInfo::instance().putInfo("Pending chunks", std::to_string(_toBuild.size() + _toLoad.size()));
}

void drft::system::ChunkManager::shutdown()
{
	for (auto& [_, chunk] : _chunks)
	{
		chunk.setState(spatial::ChunkState::ToSave);
		chunk.asyncSave(_registry, _serializer);
	}
}

void drft::system::ChunkManager::updateChunkStates(const CameraInfo& camera)
{
	ChunkPosition cameraChunkPosition = spatial::toChunkSpace(camera.position.tile);

	auto activeCoords = spatial::getIntCircleInRadius(cameraChunkPosition, ACTIVE_CHUNK_RADIUS_XY);

	// Ensure active chunks are active or will be built
	for (auto&& coord : activeCoords)
	{
		const ChunkPosition chunkPosition = spatial::asChunkSpace(coord);
		if (!_chunks.contains(chunkPosition))
		{
			_chunks.emplace(chunkPosition, spatial::VirtualChunk{ chunkPosition });
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

		if (isWithinChunkSaveDisk(coord, cameraChunkPosition)) continue;

		_toSave.push_back(coord);
		chunk.setState(spatial::ChunkState::ToSave);
	}
}

void drft::system::ChunkManager::cleanUpChunks()
{
	auto& grid = _registry.ctx().get<spatial::WorldGrid&>();
	for (auto&& chunkPosition : _toDelete)
	{
		grid.removeChunk(chunkPosition);
		_chunks.erase(chunkPosition);
	}
	_toDelete.clear();
}

void drft::system::ChunkManager::processBuildQueue()
{
	if (_toBuild.empty()) return;

	std::vector<ChunkPosition> toRemove;
	for (auto&& coord : _toBuild)
	{
		spatial::VirtualChunk& chunk = _chunks.at(coord);

		spatial::ioStatus status = chunk.build(_registry);
		if (status == spatial::ioStatus::Done)
		{
			toRemove.push_back(coord);
		}
	}
	for (auto&& coord : toRemove)
	{
		std::erase(_toBuild, coord);
	}
}

void drft::system::ChunkManager::processLoadQueue()
{
	if (_toLoad.empty()) return;

	std::vector<ChunkPosition> toRemove;
	for (auto&& coord : _toLoad)
	{
		spatial::VirtualChunk& chunk = _chunks.at(coord);

		spatial::ioStatus status = chunk.asyncLoad(_registry, _serializer);
		if (status == spatial::ioStatus::Done)
		{
			toRemove.push_back(coord);
		}
	}
	for (auto&& coord : toRemove)
	{
		std::erase(_toLoad, coord);
	}
}

void drft::system::ChunkManager::processSaveQueue()
{
	if (_toSave.empty()) return;

	std::vector<ChunkPosition> toRemove;
	for (auto&& coord : _toSave)
	{
		spatial::VirtualChunk& chunk = _chunks.at(coord);

		spatial::ioStatus status = chunk.asyncSave(_registry, _serializer);
		if (status == spatial::ioStatus::Done)
		{
			toRemove.push_back(coord);
		}
	}
	for (auto&& coord : toRemove)
	{
		std::erase(_toSave, coord);
		_toDelete.push_back(coord);
	}
}

void drft::system::ChunkManager::loadOrBuildChunk(ChunkPosition position, spatial::VirtualChunk& chunk)
{
	if (_serializer.isSerialized(position))
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

bool drft::system::ChunkManager::isWithinChunkSaveDisk(sf::Vector3i chunkPosition, sf::Vector3i centerPosition) const
{
	const int dz_abs = std::abs(chunkPosition.z - centerPosition.z);
	if (dz_abs == 0)
	{
		return spatial::isWithinRadius2d({ centerPosition.x, centerPosition.y }, { chunkPosition.x, chunkPosition.y }, TO_SAVE_CHUNK_RADIUS_XY);
	}
	return false;
}
