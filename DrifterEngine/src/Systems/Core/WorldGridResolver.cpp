#include "pch.h"
#include "WorldGridResolver.h"
#include "Components/Components.h"
#include "Spatial/WorldGrid.h"
#include "Spatial/Conversions.h"


void drft::system::WorldGridResolver::init()
{
	registry->on_construct<component::Position>().connect<&WorldGridResolver::onPositionAdd>(this);
	registry->on_update<component::Position>().connect<&WorldGridResolver::onPositionUpdate>(this);
	registry->on_destroy<component::Position>().connect<&WorldGridResolver::onPositionRemove>(this);
}

void drft::system::WorldGridResolver::onPositionAdd(entt::registry& registry, entt::entity entity)
{
	if (registry.any_of<component::Camera>(entity)) return;

	auto& pos = registry.get<component::Position>(entity);
	auto& grid = registry.ctx().get<spatial::WorldGrid&>();
	grid.placeEntity(entity, spatial::toTileSpace(pos.position));
}

void drft::system::WorldGridResolver::onPositionUpdate(entt::registry& registry, entt::entity entity)
{
	if (registry.any_of<component::Camera>(entity)) return;

	auto& pos = registry.get<component::Position>(entity);
	auto& grid = registry.ctx().get<spatial::WorldGrid&>();
	grid.moveEntity(entity, spatial::toTileSpace(pos.position));
}

void drft::system::WorldGridResolver::onPositionRemove(entt::registry& registry, entt::entity entity)
{
	if (registry.any_of<component::Camera>(entity)) return;

	auto& pos = registry.get<component::Position>(entity);
	auto& grid = registry.ctx().get<spatial::WorldGrid&>();
	grid.removeEntity(entity);
}
