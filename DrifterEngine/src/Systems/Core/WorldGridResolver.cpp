#include "pch.h"
#include "WorldGridResolver.h"
#include "Components/Components.h"
#include "Spatial/WorldGrid.h"
#include "Spatial/Conversions.h"
#include "Events/LeaveTileEvent.h"
#include "Events/EnterTileEvent.h"


void drft::system::WorldGridResolver::init()
{
	_grid = &registry->ctx().get<spatial::WorldGrid&>();
	_dispatcher = &registry->ctx().get<entt::dispatcher&>();

	registry->on_construct<component::Position>().connect<&WorldGridResolver::onPositionAdd>(this);
	registry->on_update<component::Position>().connect<&WorldGridResolver::onPositionUpdate>(this);
	registry->on_destroy<component::Position>().connect<&WorldGridResolver::onPositionRemove>(this);
}

void drft::system::WorldGridResolver::onPositionAdd(entt::registry& registry, entt::entity entity)
{
	if (registry.any_of<component::Camera>(entity)) return;

	auto& pos = registry.get<component::Position>(entity);
	_grid->placeEntity(entity, pos.position);
	_dispatcher->trigger(events::EnterTileEvent(entity, pos.position));
}

void drft::system::WorldGridResolver::onPositionUpdate(entt::registry& registry, entt::entity entity)
{
	if (registry.any_of<component::Camera>(entity)) return;

	auto& pos = registry.get<component::Position>(entity);
	const auto prevPos = _grid->getPosition(entity);
	_grid->moveEntity(entity, pos.position);
	_dispatcher->trigger(events::LeaveTileEvent(entity, prevPos));
	_dispatcher->trigger(events::EnterTileEvent(entity, pos.position));
}

void drft::system::WorldGridResolver::onPositionRemove(entt::registry& registry, entt::entity entity)
{
	if (registry.any_of<component::Camera>(entity)) return;
	_grid->removeEntity(entity);
}
