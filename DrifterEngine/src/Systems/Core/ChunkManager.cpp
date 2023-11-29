#include "pch.h"
#include "ChunkManager.h"
#include "Spatial/WorldGrid.h"
#include "Spatial/Conversions.h"
#include "Spatial/Helpers.h"
#include "Components/Components.h"
#include "ProcGen/WorldGeneration/WorldGenerator.h"
#include "Services/DebugInfo.h"
#include "Random/RandomNumberGenerator.h"

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
	auto cameraView = registry->view<const component::Camera, const component::Position>();
	sf::Vector2i cameraPosition = { 0,0 };

	for (auto [entity, cam, pos] : cameraView.each())
	{
		cameraPosition = spatial::toChunkCoordinate(pos.position);
	}

	updateChunkStates(cameraPosition);

	process(_toBuild, BUILD);
	process(_toLoad, LOAD);
	process(_toSave, SAVE);

	cleanUpChunks(cameraPosition);
}

void drft::system::ChunkManager::save(cereal::JSONOutputArchive& oarchive)
{
	for (auto& [_, chunk] : _chunks)
	{
		chunk.save(*registry, CHUNK_SAVE_PATH.data());
	}
}

void drft::system::ChunkManager::updateChunkStates(sf::Vector2i newPosition)
{
	auto activeCoords = spatial::getIntCircleInRadius(newPosition, ACTIVE_CHUNK_RADIUS);

	// Ensure active chunks are active or will be built
	for (auto&& coord : activeCoords)
	{
		auto keyablePair = std::make_pair(coord.x, coord.y);
		if (!_chunks.contains(keyablePair))
		{
			_chunks.emplace(std::make_pair(keyablePair, spatial::VirtualChunk{ coord }));
		}
		auto& chunk = _chunks.at(keyablePair);

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
		float distance = std::hypotf(static_cast<float>((newPosition.x - coord.first)),
									static_cast<float>((newPosition.y - coord.second)));
		if (distance > TO_SAVE_CHUNK_RADIUS)
		{
			_toSave.push({ coord.first, coord.second });
			chunk.setState(spatial::ChunkState::ToSave);
		}
	}
}

void drft::system::ChunkManager::cleanUpChunks(sf::Vector2i newPosition)
{
	std::vector<std::pair<int, int>> toDelete;
	for (auto& [coord, chunk] : _chunks)
	{
		if (chunk.getState() != spatial::ChunkState::Saved)
		{
			continue;
		}
		float distance = std::hypotf(static_cast<float>((newPosition.x - coord.first)),
									static_cast<float>((newPosition.y - coord.second)));
		if (distance > TO_SAVE_CHUNK_RADIUS)
		{
			toDelete.push_back(coord);
		}
	}
	auto& grid = registry->ctx().get<spatial::WorldGrid&>();
	for (auto pair : toDelete)
	{
		grid.removeChunk({ pair.first, pair.second });
		_chunks.erase(pair);
		std::cout << "Chunk " << pair.first << "_" << pair.second << " cleaned up." << std::endl;
	}
}

void drft::system::ChunkManager::process(std::queue<sf::Vector2i>& chunkQueue, ProcessType type)
{
	if (chunkQueue.empty()) return;
	
	std::vector<std::pair<int, int>> toDelete;

	auto coord = chunkQueue.front();
	auto keyablePair = std::make_pair(coord.x, coord.y);
	auto status = spatial::ioStatus::Busy;

	switch (type)
	{
	case BUILD:
		status = _chunks.at(keyablePair).build(*registry);
		break;
	case SAVE:
		status = _chunks.at(keyablePair).asyncSave(*registry, CHUNK_SAVE_PATH.data());
		break;
	case LOAD:
		status = _chunks.at(keyablePair).asyncLoad(*registry, CHUNK_SAVE_PATH.data());
		break;
	}
	if (status == spatial::ioStatus::Busy)
	{
		auto temp = chunkQueue.front();
		chunkQueue.push(temp);
	}
	chunkQueue.pop();
}