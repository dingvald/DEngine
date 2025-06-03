#include "pch.h"
#include "WorldGridResolver.h"
#include "Components/CameraComponent.h"
#include "Components/PositionComponent.h"
#include "Components/VisualEffectComponent.h"
#include "Spatial/WorldGrid.h"
#include "Events/LeaveTileEvent.h"
#include "Events/EnterTileEvent.h"

namespace Internal
{
	static bool shouldExclude(entt::registry& registry, entt::entity entity)
	{
		return registry.any_of<
			CameraComponent, 
			VisualEffectComponent
		>(entity);
	}
}

void drft::system::WorldGridResolver::init()
{
	_grid = &_registry.ctx().get<spatial::WorldGrid&>();

	_registry.on_construct<PositionComponent>().connect<&WorldGridResolver::onPositionAdd>(this);
	_registry.on_update<PositionComponent>().connect<&WorldGridResolver::onPositionUpdate>(this);
	_registry.on_destroy<PositionComponent>().connect<&WorldGridResolver::onPositionRemove>(this);
}

void drft::system::WorldGridResolver::onPositionAdd(entt::registry& registry, entt::entity entity)
{
	if (Internal::shouldExclude(registry, entity)) return;

	auto& pos = registry.get<PositionComponent>(entity);
	_grid->placeEntity(entity, pos.tile);
	_dispatcher.trigger(events::EnterTileEvent{ entity, pos.tile });
}

void drft::system::WorldGridResolver::onPositionUpdate(entt::registry& registry, entt::entity entity)
{
	if (Internal::shouldExclude(registry, entity)) return;

	auto& pos = registry.get<PositionComponent>(entity);
	const auto prevPos = _grid->getPosition(entity);
	_grid->moveEntity(entity, pos.tile);
	_dispatcher.trigger(events::LeaveTileEvent{ entity, prevPos });
	_dispatcher.trigger(events::EnterTileEvent{ entity, pos.tile });
}

void drft::system::WorldGridResolver::onPositionRemove(entt::registry& registry, entt::entity entity)
{
	if (Internal::shouldExclude(registry, entity)) return;

	_grid->removeEntity(entity);
}
