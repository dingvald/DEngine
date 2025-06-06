#pragma once
#include "Cell.h"
#include <Spatial/Grid3d.h>

namespace drft::spatial
{
	class WorldChunk
	{
	public:
		WorldChunk(sf::Vector3i dimensions);

		// Places entity in chunk tile position.
		void placeEntity(entt::entity entity, sf::Vector3i position);
		// Removes entity from given chunk tile position and returns that entity.
		void removeEntity(entt::entity entity, sf::Vector3i position);

		const std::vector<entt::entity>& entitiesAt(sf::Vector3i chunkPosition) const;
		std::vector<entt::entity> getAllEntities() const;

		// Checks if the chunk contains any entities
		bool empty() const;

	private:
		entt::dense_set<entt::entity> _entities;
		Grid3d<Cell> _grid = { sf::Vector3i{8, 8, 8} };
	};

} // namespace drft::spatial

