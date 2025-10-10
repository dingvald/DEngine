#pragma once

#include <Spatial/ChunkPosition.h>
#include <Generation/GenerationState.h>

struct GenerationRegistries;

namespace drft
{
	class GenerationLayerManager;
}

class ChunkGenerator
{
public:
	ChunkGenerator();
	~ChunkGenerator();

	void init(entt::registry& registry);
	void setGenerationLayers(drft::GenerationLayerManager& layers);

	[[nodiscard]] std::future<entt::registry> queueForGeneration(drft::ChunkPosition chunkPosition, entt::registry&& registry);
	void queueCleanup(drft::ChunkPosition chunkPosition);

private:
	void flushPendingGenerations();

	void generationThread();

	void syncPendingQueue();
	void processQueue();

	void processCleanup();

	GenerationState generateChunk(drft::ChunkPosition position, entt::registry& registry);

private:
	struct ChunkRegistryPair
	{
		drft::ChunkPosition position;
		entt::registry registry;
	};

	bool _hasBeenFinalized = false;
	drft::GenerationLayerManager* _layers = nullptr;

	std::thread _generationThread;
	std::atomic_bool _shouldShutdown = false;

	std::mutex _pendingQueueLock;
	std::vector<ChunkRegistryPair> _pendingQueue;
	std::vector<ChunkRegistryPair> _generationQueue;

	std::mutex _promiseLock;
	std::unordered_map<drft::ChunkPosition, std::promise<entt::registry>> _promises;

	std::mutex _cleanupLock;
	std::unordered_set<sf::Vector3i> _cleanupSet;
	std::vector<sf::Vector3i> _cleanupPositions;
};