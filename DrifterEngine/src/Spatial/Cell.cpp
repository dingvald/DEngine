#include "pch.h"
#include "Cell.h"

void drft::spatial::Cell::placeEntity(entt::entity entity)
{
	_entities.push_back(entity);
}

void drft::spatial::Cell::removeEntity(entt::entity entity)
{
	_entities.erase(std::remove(_entities.begin(), _entities.end(), entity), _entities.end());
}

const std::vector<entt::entity>& drft::spatial::Cell::getEntities() const
{
	return _entities;
}

bool drft::spatial::Cell::isEmpty() const
{
	return _entities.empty();
}
