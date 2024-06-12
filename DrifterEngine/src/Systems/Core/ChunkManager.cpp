#include "pch.h"
#include "ChunkManager.h"
#include "Spatial/WorldGrid.h"
#include "Spatial/Conversions.h"
#include "Spatial/Helpers.h"
#include "Components/CameraComponent.h"
#include "Components/PositionComponent.h"
#include "ProcGen/WorldGeneration/WorldGenerator.h"
#include "Services/DebugInfo.h"
#include "Random/RandomNumberGenerator.h"
#include "Systems/Helpers/GetCurrentCamera.h"

using namespace drft::system;

// TODO: Move save directory to state context
static constexpr std::string_view CHUNK_SAVE_PATH = ".\\data\\savegame\\chunks\\";

static constexpr int ACTIVE_CHUNK_RADIUS = 2;
static constexpr int TO_SAVE_CHUNK_RADIUS = ACTIVE_CHUNK_RADIUS + 1;

void drft::system::ChunkManager::init()
{
}

void drft::system::ChunkManager::update(const float dt)
{
	auto camera = getCurrentCamera(*_registry);
	sf::Vector2i cameraChunkPosition = spatial::toChunkCoordinate(camera.position);

	updateChunkStates(cameraChunkPosition);

	process(_toBuild, BUILD);
	process(_toLoad, LOAD);
	process(_toSave, SAVE);

	cleanUpChunks(cameraChunkPosition);
}

void drft::system::ChunkManager::save(cereal::JSONOutputArchive& oarchive)
{
	for (auto& [_, chunk] : _chunks)
	{
		chunk.save(*_registry, CHUNK_SAVE_PATH.data());
	}
}

void drft::system::ChunkManager::updateChunkStates(sf::Vector2i newPosition)
{
	auto activeCoords = spatial::getIntCircleInRadius(newPosition, ACTIVE_CHUNK_RADIUS);

	// Ensure active chunks are active or will be built
	for (auto&& coord : activeCoords)
	{
		if (!_chunks.contains(coord))
		{
			_chunks.emplace(coord, spatial::VirtualChunk{ coord });
		}
		auto& chunk = _chunks.at(coord);

		switch (chunk.getState())
		{
		case spatial::ChunkState::None:
			if (std::filesystem::exists(CHUNK_SAVE_PATH.data() + chunk.toString() + ".dat"))
			{
				chunk.setState(spatial::ChunkState::ToLoad);
				_toLoad.push(coord);
			}
			else
			{
				chunk.setState(spatial::ChunkState::ToBuild);
				_toBuild.push(coord);
			}
			break;
		case spatial::ChunkState::Built:
			chunk.setState(spatial::ChunkState::Active);
			break;
		case spatial::ChunkState::Loaded:
			chunk.setState(spatial::ChunkState::Active);
			break;
		case spatial::ChunkState::Saved:
			chunk.setState(spatial::ChunkState::ToLoad);
			_toLoad.push(coord);
			break;
		default:
			break;
		}
	}

	// Then, scan for chunks to save
	for (auto& [coord, chunk] : _chunks)
	{
		if (chunk.getState() != spatial::ChunkState::Active)
		{
			continue;
		}
		float distance = spatial::distance(coord, newPosition);
		if (distance > TO_SAVE_CHUNK_RADIUS)
		{
			_toSave.push(coord);
			chunk.setState(spatial::ChunkState::ToSave);
		}
	}
}

void drft::system::ChunkManager::cleanUpChunks(sf::Vector2i newPosition)
{
	std::vector<sf::Vector2i> toDelete;
	for (auto& [coord, chunk] : _chunks)
	{
		if (chunk.getState() != spatial::ChunkState::Saved)
		{
			continue;
		}
		float distance = spatial::distance(coord, newPosition);
		if (distance > TO_SAVE_CHUNK_RADIUS)
		{
			toDelete.push_back(coord);
		}
	}
	auto& grid = _registry->ctx().get<spatial::WorldGrid&>();
	for (auto&& coord : toDelete)
	{
		grid.removeChunk(coord);
		_chunks.erase(coord);
		std::cout << "Chunk " << coord.x << "_" << coord.y << " cleaned up." << std::endl;
	}
}

void drft::system::ChunkManager::process(std::queue<sf::Vector2i>& chunkQueue, ProcessType type)
{
	if (chunkQueue.empty()) return;
	
	std::vector<std::pair<int, int>> toDelete;

	auto coord = chunkQueue.front();
	auto status = spatial::ioStatus::Busy;

	switch (type)
	{
	case BUILD:
		status = _chunks.at(coord).build(*_registry);
		break;
	case SAVE:
		status = _chunks.at(coord).asyncSave(*_registry, CHUNK_SAVE_PATH.data());
		break;
	case LOAD:
		status = _chunks.at(coord).asyncLoad(*_registry, CHUNK_SAVE_PATH.data());
		break;
	}
	if (status == spatial::ioStatus::Busy)
	{
		auto temp = chunkQueue.front();
		chunkQueue.push(temp);
	}
	chunkQueue.pop();
}