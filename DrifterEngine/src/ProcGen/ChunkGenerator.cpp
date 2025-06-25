#include "pch.h"
#include "ChunkGenerator.h"

#include <ProcGen/Layers/EntityPlacementLayer.h>
#include <ProcGen/Layers/TilePlacementLayer.h>

#include <Spatial/Conversions.h>

using namespace entt::literals;

ChunkGenerator::ChunkGenerator()
{
	_generationThread = std::thread{ &ChunkGenerator::generationThread, this };
}

ChunkGenerator::~ChunkGenerator()
{
	_shouldShutdown = true;
	if (_generationThread.joinable())
	{
		_generationThread.join();
	}
}

void ChunkGenerator::flushPendingGenerations()
{
	std::lock_guard<std::mutex> lock{ _pendingQueueLock };
	_pendingQueue.clear();
}

void ChunkGenerator::setGenerationLayers(drft::GenerationLayerManager& layers)
{
	flushPendingGenerations();
	_layers = &layers;
}

std::future<entt::registry> ChunkGenerator::queueForGeneration(drft::ChunkPosition chunkPosition, entt::registry&& registry)
{
	{
		std::lock_guard<std::mutex> lock{ _pendingQueueLock };
		_pendingQueue.emplace_back(chunkPosition, std::move(registry));
	}

	std::lock_guard<std::mutex> lock{ _promiseLock };
	_promises.emplace(chunkPosition, std::promise<entt::registry>{});
	return _promises.at(chunkPosition).get_future();
}

void ChunkGenerator::queueCleanup(drft::ChunkPosition chunkPosition)
{
	std::lock_guard<std::mutex> lock{ _cleanupLock };
	if (!_cleanupSet.contains(chunkPosition))
	{
		_cleanupSet.insert(chunkPosition);
		_cleanupPositions.push_back(chunkPosition);
	}
}

void ChunkGenerator::generationThread()
{
	do
	{
		try {
			syncPendingQueue();
			processQueue();
			processCleanup();
		}
		catch (std::exception e)
		{
			LOG_ERROR("Error in generation thread: {}", e.what());
		}
	} while (!_shouldShutdown || !_generationQueue.empty());
}

void ChunkGenerator::syncPendingQueue()
{
	std::lock_guard<std::mutex> lock{ _pendingQueueLock };
	
	for (auto&& [pos, reg] : _pendingQueue)
	{
		_generationQueue.emplace_back(pos, std::move(reg));
	}
	_pendingQueue.clear();
}

void ChunkGenerator::processCleanup()
{
	if (!_layers) return;

	std::lock_guard<std::mutex> lock{ _cleanupLock };

	for (auto&& position : _cleanupPositions)
	{
		_layers->cleanup(position);
	}
	_cleanupPositions.clear();
	_cleanupSet.clear();
}

GenerationState ChunkGenerator::generateChunk(drft::ChunkPosition position, entt::registry& registry)
{
	const sf::Vector3i origin = drft::spatial::toTileSpace(position);
	drft::spatial::AABB<int> volume = { origin, drft::ChunkDimensions };

	auto tilePlacementLayer = _layers->generate<drft::TilePlacementLayer>(volume);
	if (!tilePlacementLayer.isReady()) return tilePlacementLayer.getState();
	
	auto entityPlacementLayer = _layers->generate<drft::EntityPlacementLayer>(volume);
	if (!entityPlacementLayer.isReady()) return entityPlacementLayer.getState();
	
	tilePlacementLayer.unwrap().placeTiles(volume, registry);
	entityPlacementLayer.unwrap().placeEntities(volume, registry);

	auto& slotCanvas = _layers->getCanvas("slot_canvas"_hs);
	slotCanvas.discard(origin);

	return GenerationState::Complete;
}

void ChunkGenerator::processQueue()
{
	if (!_layers) return;
	if (_generationQueue.empty()) return;

	std::vector<ChunkRegistryPair> newQueue;
	newQueue.reserve(_generationQueue.size());
	for (auto&& [pos, reg] : _generationQueue)
	{
		auto state = generateChunk(pos, reg);
		if (state == GenerationState::Complete)
		{
			// Set the promise and do not re-queue
			_promises.at(pos).set_value(std::move(reg));
			_promises.erase(pos);
		}
		else
		{
			newQueue.emplace_back(pos, std::move(reg));
		}
	}
	_generationQueue.swap(newQueue);
	_generationQueue.shrink_to_fit();
}
