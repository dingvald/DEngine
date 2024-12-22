#pragma once
#include <Spatial/WorldChunk.h>
#include <Spatial/ChunkPosition.h>
#include <Spatial/TilePosition.h>

namespace drft::spatial
{	
	using EntityList = std::vector<entt::entity>;

	// Class for single point of access to query/manipulate entity and chunk positions
	class WorldGrid
	{
	public:
		WorldGrid() = default;
		WorldGrid(const WorldGrid&) = delete;
		WorldGrid& operator=(const WorldGrid&) = delete;
		WorldGrid(const WorldGrid&&) = delete;
		WorldGrid& operator=(const WorldGrid&&) = delete;

		///// Entities /////

		// Places entity in world tile position.
		void placeEntity(entt::entity entity, TilePosition tilePosition);

		// Removes entity from given world tile position.
		void removeEntity(entt::entity entity);

		// Moves entity from one world tile position to another.
		void moveEntity(entt::entity, TilePosition toTilePosition);

		// Gets the world tile position of the given entity.
		TilePosition getPosition(entt::entity entity) const;

		// Returns all entities at the given world tile position.
		const EntityList& entitiesAt(TilePosition tilePosition) const;
		// Returns entities at the given world tile position using the provided filter function.
		EntityList entitiesAt(TilePosition tilePosition, std::function<bool(entt::entity)> filterFunc) const;

		///// Chunks //////

		// Clears the chunk of all entities and removes the chunk.
		void removeChunk(ChunkPosition coordinate);

		// Returns all entities at the given chunk coordinate.
		EntityList getAllEntities(ChunkPosition coordinate) const;

		///// Ray Casting /////

		// Cast a ray from origin to destination that returns all entities encountered along the cast
		// NOTE: Does not include the origin in the cast
		std::vector<entt::entity> castRay(TilePosition origin, TilePosition destination) const;
		// Cast a ray from origin to destination that returns all entities encountered along the cast
		// Pass in a filter function to only filter which entities are returned
		// NOTE: Does not include the origin in the cast
		std::vector<entt::entity> castRay(TilePosition origin, TilePosition destination, std::function<bool(entt::entity)> filterFunc) const;

		///// Pathfinding /////
		using Heuristic = std::function<int(const std::vector<entt::entity>&)>;
		// Produces a path from pt1 to pt2.
		// Operates at the Z-Level of point 1
		std::deque<sf::Vector3i> getPath(TilePosition pt1, TilePosition pt2, Heuristic h = [](const EntityList&){return 0;}) const;

	private:
		std::unordered_map<ChunkPosition, WorldChunk> _chunks;
		std::unordered_map<entt::entity, TilePosition> _entityPositions;
	};

} // Namespace drft::spatial