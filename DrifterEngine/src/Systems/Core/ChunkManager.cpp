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
static constexpr int TO_SAVE_CHUNK_RADIUS_XY = ACTIVE_CHUNK_RADIUS_XY + 10;

void drft::system::ChunkManager::onUpdate(const float dt)
{
	const CameraInfo camera = getCurrentCamera(_registry);
	if (!camera.isInitialized) return;

	updateChunkStates(camera);

	processBuildQueue();
	processLoadQueue();
	processSaveQueue();

	cleanUpChunks();

	service::DebugInfo::instance().putInfo("Active chunks", std::to_string(_chunks.size()));
}

void drft::system::ChunkManager::shutdown()
{
	for (auto& [_, chunk] : _chunks)
	{
		chunk.save(_registry, buildChunkFilename(chunk));
	}
}

void drft::system::ChunkManager::updateChunkStates(const CameraInfo& camera)
{
	ChunkPosition cameraChunkPosition = spatial::toChunkSpace(spatial::asTileSpace(camera.position));

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
	for (auto& [coord, chunk] : _chunks)
	{
		if (chunk.getState() != spatial::ChunkState::Active) continue;

		if (isWithinChunkSaveDisk(coord, cameraChunkPosition)) continue;

		_toSave.push(coord);
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

	ChunkPosition coord = _toBuild.front();
	auto status = spatial::ioStatus::Busy;
	spatial::VirtualChunk& chunk = _chunks.at(coord);
	status = chunk.build(_registry);

	// Always build in order
	if (status == spatial::ioStatus::Done)
	{
		_toBuild.pop();
	}
}

void drft::system::ChunkManager::processLoadQueue()
{
	if (_toLoad.empty()) return;

	ChunkPosition coord = _toLoad.front();
	auto status = spatial::ioStatus::Busy;
	spatial::VirtualChunk& chunk = _chunks.at(coord);
	status = chunk.asyncLoad(_registry, buildChunkFilename(chunk));

	// Always load in order
	if (status == spatial::ioStatus::Done)
	{
		_toLoad.pop();
	}
}

void drft::system::ChunkManager::processSaveQueue()
{
	if (_toSave.empty()) return;

	ChunkPosition coord = _toSave.front();
	auto status = spatial::ioStatus::Busy;
	spatial::VirtualChunk& chunk = _chunks.at(coord);
	status = chunk.asyncSave(_registry, buildChunkFilename(chunk));

	// Can save out of order
	if (status == spatial::ioStatus::Busy)
	{
		// Send to the back of the queue
		ChunkPosition temp = _toSave.front();
		_toSave.push(temp);
	}
	else
	{
		_toDelete.push_back(coord);
	}
	_toSave.pop();
}

void drft::system::ChunkManager::loadOrBuildChunk(ChunkPosition position, spatial::VirtualChunk& chunk)
{
	if (std::filesystem::exists(buildChunkFilename(chunk)))
	{
		chunk.setState(spatial::ChunkState::ToLoad);
		_toLoad.push(std::move(position));
	}
	else
	{
		chunk.setState(spatial::ChunkState::ToBuild);
		_toBuild.push(std::move(position));
	}
}

std::filesystem::path drft::system::ChunkManager::buildChunkFilename(const spatial::VirtualChunk& chunk) const
{
	std::filesystem::path chunkFileName = CHUNK_DIRECTORY / chunk.toString();
	return chunkFileName += ".dat";
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
