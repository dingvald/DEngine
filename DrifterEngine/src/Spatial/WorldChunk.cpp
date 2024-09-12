#include "pch.h"
#include "WorldChunk.h"

drft::spatial::WorldChunk::WorldChunk(sf::Vector3i dimensions)
	: _grid(dimensions)
{}

void drft::spatial::WorldChunk::placeEntity(entt::entity entity, sf::Vector3i position)
{
	auto &cell = _grid.at(position);
	cell.placeEntity(entity);
	_entities.insert(entity);
}

void drft::spatial::WorldChunk::removeEntity(entt::entity entity, sf::Vector3i position)
{
	auto& cell = _grid.at(position);
	cell.removeEntity(entity);
	_entities.erase(entity);
}

bool drft::spatial::WorldChunk::moveEntity(entt::entity entity, sf::Vector3i fromPosition, sf::Vector3i toPosition)
{
	auto& fromCell = _grid.at(fromPosition);
	fromCell.removeEntity(entity);
	auto& toCell = _grid.at(toPosition);
	toCell.placeEntity(entity);

	return true;
}

const std::vector<entt::entity>& drft::spatial::WorldChunk::entitiesAt(sf::Vector3i position) const
{
	const Cell& cell = _grid.at(position);
	return cell.getEntities();
}

std::vector<entt::entity> drft::spatial::WorldChunk::getAllEntities() const
{
	return std::vector<entt::entity>(_entities.begin(), _entities.end());
}

bool drft::spatial::WorldChunk::empty() const
{
	return _entities.empty();
}