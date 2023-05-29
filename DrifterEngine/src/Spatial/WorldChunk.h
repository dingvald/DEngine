#pragma once
#include "Cell.h"
#include "Grid.h"


namespace drft::spatial
{

	class WorldChunk
	{
	public:
		WorldChunk(int width, int height);

		// Places entity in chunk tile position.
		void placeEntity(const entt::entity& entity, const sf::Vector2i chunkPosition);
		// Removes entity from given chunk tile position and returns that entity.
		void removeEntity(const entt::entity& entity, const sf::Vector2i chunkPosition);
		// Moves entity from one chunk tile position to another and returns if successful.
		bool moveEntity(const entt::entity, const sf::Vector2i fromChunkPosition, const sf::Vector2i toChunkPosition);
		std::vector<entt::entity> entitiesAt(sf::Vector2i chunkPosition) const;
		std::vector<entt::entity> getAllEntities();
		// Checks if the chunk contains any entities
		bool empty() const;

	private:
		std::set<entt::entity> _entities;
		Grid<Cell> _grid = { 64, 64 };
	};

} // namespace drft::spatial

