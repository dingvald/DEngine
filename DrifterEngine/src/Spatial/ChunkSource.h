#pragma once
#include "Spatial/VirtualChunk.h"

class drft::ChunkSerializer;
class ChunkGenerator;

class IChunkGenerator;

namespace drft::spatial
{
	class WorldGrid;

	class ChunkSource
	{
	public:
		ChunkSource(entt::id_type sourceId, ChunkSerializer& serializer, ChunkGenerator& generator);

		void init(entt::registry& registry);
		void update(TilePosition position, entt::registry& registry);
		void updateEnd(TilePosition cameraPosition);
		void shutdown(entt::registry& registry, bool isAsync);

		bool isLoadedAroundPosition(TilePosition position) const;
		entt::id_type id() const;

	private:
		void updateChunkStates(TilePosition position);
		void cleanUpSavedChunks(entt::registry& registry);
		void cleanUpAllChunks(entt::registry& registry);

		bool processBuildQueue(entt::registry& registry);
		bool processLoadQueue(entt::registry& registry);
		bool processSaveQueue(entt::registry& registry);

		void loadOrBuildChunk(ChunkPosition position, spatial::VirtualChunk& chunk);
		void saveChunk(ChunkPosition position, spatial::VirtualChunk& chunk);

		bool isWithinActiveArea(sf::Vector3i chunkPosition, sf::Vector3i centerPosition) const;

	private:
		entt::id_type _sourceId;
		bool _isShuttingDown = false;

		ChunkSerializer& _serializer;
		ChunkGenerator& _generator;
		std::unordered_map<ChunkPosition, spatial::VirtualChunk> _chunks;

		std::vector<ChunkPosition> _toBuild;
		std::vector<ChunkPosition> _toLoad;
		std::vector<ChunkPosition> _toSave;
		std::vector<ChunkPosition> _toDelete;
	};
}