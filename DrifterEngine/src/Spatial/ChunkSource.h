#pragma once
#include "Spatial/VirtualChunk.h"
#include <Utility/ChunkSerializer.h>

namespace drft::system
{
	struct CameraHandle;
}

namespace drft::spatial
{
	class WorldGrid;

	class ChunkSource
	{
	public:
		ChunkSource(entt::id_type sourceId, ChunkSerializer& serializer);

		void update(TilePosition cameraPosition, entt::registry& registry);
		void shutdown(entt::registry& registry);
		bool isReady() const;
		entt::id_type id() const;

	private:
		void updateChunkStates(TilePosition cameraPosition);
		void cleanUpSavedChunks(entt::registry& registry);
		void cleanUpAllChunks(entt::registry& registry);

		void flushAllQueues(entt::registry& registry);
		bool processBuildQueue(entt::registry& registry);
		bool processLoadQueue(entt::registry& registry);
		bool processSaveQueue(entt::registry& registry);

		void loadOrBuildChunk(ChunkPosition position, spatial::VirtualChunk& chunk);

		bool isWithinChunkSaveArea(sf::Vector3i chunkPosition, sf::Vector3i centerPosition) const;


	private:
		entt::id_type _sourceId;
		bool _isFlushed = true; // has nothing in any queue
		bool _isShuttingDown = false;
		ChunkSerializer& _serializer;
		std::unordered_map<ChunkPosition, spatial::VirtualChunk> _chunks;

		std::vector<ChunkPosition> _toBuild;
		std::vector<ChunkPosition> _toLoad;
		std::vector<ChunkPosition> _toSave;
		std::vector<ChunkPosition> _toDelete;
	};
}