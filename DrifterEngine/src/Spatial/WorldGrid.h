#pragma once
#include "WorldChunk.h"

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
		void placeEntity(const entt::entity entity, const sf::Vector2i tilePosition);

		// Removes entity from given world tile position.
		void removeEntity(const entt::entity entity);

		// Moves entity from one world tile position to another.
		void moveEntity(const entt::entity, const sf::Vector2i toTilePosition);

		// Gets the world tile position of the given entity.
		sf::Vector2i getPosition(const entt::entity entity) const;

		// Returns all entities at the given world tile position.
		EntityList entitiesAt(const sf::Vector2i tilePosition) const;
		// Returns entities at the given world tile position using the provided filter function.
		EntityList entitiesAt(const sf::Vector2i tilePosition, std::function<bool(entt::entity)> filterFunc) const;

		///// Chunks //////

		// Clears the chunk of all entities and removes the chunk.
		void removeChunk(const sf::Vector2i coordinate);

		// Returns all entities at the given chunk coordinate.
		EntityList getAllEntities(const sf::Vector2i coordinate) const;

		///// Ray Casting /////

		// Cast a ray from origin to destination that returns all entities encountered along the cast
		// NOTE: Does not include the origin in the cast
		std::vector<entt::entity> castRay(sf::Vector2i origin, sf::Vector2i destination) const;
		// Cast a ray from origin to destination that returns all entities encountered along the cast
		// Pass in a filter function to only filter which entities are returned
		// NOTE: Does not include the origin in the cast
		std::vector<entt::entity> castRay(sf::Vector2i origin, sf::Vector2i destination, std::function<bool(entt::entity)> filterFunc) const;

		///// Pathfinding /////
		using heuristic = std::function<int(const std::vector<entt::entity>&)>;
		// Produces a path from pt1 to pt2.
		std::deque<sf::Vector2i> getPath(sf::Vector2i pt1, sf::Vector2i pt2, heuristic h = [](const EntityList&){return 0;}) const;

	private:
		std::map<std::pair<int, int>, std::unique_ptr<WorldChunk>> _chunks;
		std::unordered_map<entt::entity, sf::Vector2i> _entityPositions;
	};

} // Namespace drft::spatial