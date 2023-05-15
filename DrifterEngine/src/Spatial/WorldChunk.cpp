#include "pch.h"
#include "WorldChunk.h"

drft::spatial::WorldChunk::WorldChunk(int width, int height)
{
	_grid.resize(width, height);
}

void drft::spatial::WorldChunk::placeEntity(const entt::entity& entity, const sf::Vector2i chunkPosition, int layer)
{
	auto &cell = _grid.at(chunkPosition.x, chunkPosition.y);
	cell.placeEntity(entity, layer);
	_entities.insert(entity);
}

entt::entity drft::spatial::WorldChunk::removeEntity(const entt::entity& entity, const sf::Vector2i chunkPosition, int layer)
{
	entt::entity result = entt::null;

	auto& cell = _grid.at(chunkPosition.x, chunkPosition.y);
	result = cell.removeEntity(entity, layer);
	_entities.erase(entity);

	return result;
}

bool drft::spatial::WorldChunk::moveEntity(const entt::entity entity, const sf::Vector2i fromChunkPosition, const sf::Vector2i toChunkPosition, int layer)
{
	auto& fromCell = _grid.at(fromChunkPosition.x, fromChunkPosition.y);

	entt::entity e = fromCell.removeEntity(entity, layer);
	if (e == entt::null) return false;

	auto& toCell = _grid.at(toChunkPosition.x, toChunkPosition.y);
	toCell.placeEntity(e, layer);

	return true;
}

std::vector<entt::entity> drft::spatial::WorldChunk::entitiesAt(sf::Vector2i chunkPosition, int layer) const
{
	auto& cell = _grid.at(chunkPosition.x, chunkPosition.y);
	return cell.at(layer);;
}

std::vector<entt::entity> drft::spatial::WorldChunk::getAllEntities()
{
	return std::vector<entt::entity>(_entities.begin(), _entities.end());
}

bool drft::spatial::WorldChunk::empty() const
{
	return _entities.empty();
}