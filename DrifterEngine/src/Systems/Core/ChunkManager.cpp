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

static const std::filesystem::path WORKING_DIRECTORY = ".";
static const std::filesystem::path SAVE_DIRECTORY = WORKING_DIRECTORY / "data" / "savegame";
static const std::filesystem::path CHUNK_DIRECTORY = SAVE_DIRECTORY / "chunks";

static constexpr int ACTIVE_CHUNK_RADIUS = 10;
static constexpr int TO_SAVE_CHUNK_RADIUS = ACTIVE_CHUNK_RADIUS + 10;


void drft::system::ChunkManager::onUpdate(const float dt)
{
	auto camera = getCurrentCamera(_registry);
	if (!camera.isInitialized) return;

	sf::Vector2i cameraChunkPosition = spatial::toChunkCoordinate(camera.position);

	updateChunkStates(cameraChunkPosition);

	process(_toBuild, BUILD);
	process(_toLoad, LOAD);
	process(_toSave, SAVE);

	cleanUpChunks(cameraChunkPosition);

	service::DebugInfo::instance().putInfo("Active chunks", std::to_string(_chunks.size()));
}

void drft::system::ChunkManager::save(cereal::JSONOutputArchive& oarchive)
{
	for (auto& [_, chunk] : _chunks)
	{
		chunk.save(_registry, buildChunkFilename(chunk));
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
			if (std::filesystem::exists(buildChunkFilename(chunk)))
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
		if (distance < TO_SAVE_CHUNK_RADIUS) [[likely]] continue;

		_toSave.push(coord);
		chunk.setState(spatial::ChunkState::ToSave);
	}
}

void drft::system::ChunkManager::cleanUpChunks(sf::Vector2i newPosition)
{
	std::vector<sf::Vector2i> toDelete;
	for (auto& [coord, chunk] : _chunks)
	{
		if (chunk.getState() != spatial::ChunkState::Saved) continue;

		const float distance = spatial::distance(coord, newPosition);
		if (distance < TO_SAVE_CHUNK_RADIUS) [[likely]] continue;

		toDelete.push_back(coord);
	}

	auto& grid = _registry.ctx().get<spatial::WorldGrid&>();
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

	sf::Vector2i coord = chunkQueue.front();
	auto status = spatial::ioStatus::Busy;
	spatial::VirtualChunk& chunk = _chunks.at(coord);

	switch (type)
	{
	case BUILD:
		status = chunk.build(_registry);
		break;
	case SAVE:
		status = chunk.asyncSave(_registry, buildChunkFilename(chunk));
		break;
	case LOAD:
		status = chunk.asyncLoad(_registry, buildChunkFilename(chunk));
		break;
	}

	if (status == spatial::ioStatus::Busy)
	{
		// Send to the back of the queue
		sf::Vector2i temp = chunkQueue.front();
		chunkQueue.push(temp);
	}
	chunkQueue.pop();
}

std::filesystem::path drft::system::ChunkManager::buildChunkFilename(const spatial::VirtualChunk& chunk) const
{
	std::filesystem::path chunkFileName = CHUNK_DIRECTORY / chunk.toString();
	return chunkFileName += ".dat";
}
