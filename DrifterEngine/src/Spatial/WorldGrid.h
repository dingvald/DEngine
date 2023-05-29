#pragma once
#include "WorldChunk.h"

namespace drft::spatial
{	
	using EntityList = std::vector<entt::entity>;

	// Class for single point of access to query/manipulate entity and chunk positions
	class WorldGrid
	{
	public:
		// Creates a world grid containing chunks with the given dimensions.
		WorldGrid() = default;
		WorldGrid(const WorldGrid&) = delete;
		WorldGrid& operator=(const WorldGrid&) = delete;
		WorldGrid(const WorldGrid&&) = delete;
		WorldGrid& operator=(const WorldGrid&&) = delete;

		// Entities //

		// Places entity in world tile position.
		void placeEntity(const entt::entity entity, const sf::Vector2i tilePosition);

		// Removes entity from given world tile position.
		void removeEntity(const entt::entity entity);

		// Moves entity from one world tile position to another and returns if successful.
		bool moveEntity(const entt::entity, const sf::Vector2i toTilePosition);

		// Gets the world tile position of the given entity.

		const sf::Vector2i getPosition(const entt::entity entity) const;

		// Returns all entities at the given world tile position.
		const EntityList entitiesAt(const sf::Vector2i tilePosition) const;
		const EntityList entitiesAt(const sf::Vector2i tilePosition, std::function<bool(entt::entity)> filterFunc) const;

		// Chunks //

		// Clears the chunk of all entities and removes the chunk.
		void removeChunk(const sf::Vector2i coordinate);

		// Returns all entities at the given chunk coordinate.
		EntityList getAllEntities(const sf::Vector2i coordinate) const;

		// Pathfinding
		using heuristic = std::function<int(const std::vector<entt::entity>&)>;
		std::deque<sf::Vector2i> getPath(sf::Vector2i pt1, sf::Vector2i pt2, heuristic h = [](const std::vector<entt::entity>&){return 0;}) const;

	private:
		std::map<std::pair<int, int>, std::unique_ptr<WorldChunk>> _chunks;
		std::unordered_map<entt::entity, sf::Vector2i> _entityPositions;
	};

} // Namespace drft::spatial