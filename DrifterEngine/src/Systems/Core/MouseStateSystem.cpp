#include "pch.h"
#include "MouseStateSystem.h"
#include <Components/PhysicalBlockingComponent.h>
#include <Components/InteractableComponent.h>
#include <Components/MouseCursorComponent.h>
#include <Components/PositionComponent.h>

#include <Spatial/WorldGrid.h>

#include <Systems/Gameplay/FactionSystem.h>
#include <Systems/Helpers/GetMouseTilePosition.h>
#include <Systems/Helpers/GetPlayerHandle.h>

void drft::system::MouseStateSystem::init()
{

}

void drft::system::MouseStateSystem::update()
{
	auto view = _registry.view<MouseCursorComponent>();
	for (auto&& [entity, mouse] : view.each())
	{
		updateMousePosition(entity);
		updateMouseContextualState(entity);
	}
}

void drft::system::MouseStateSystem::updateMousePosition(entt::entity mouse) const
{
	auto mouseTilePosition = getMouseTilePosition(_registry);
	_registry.emplace_or_replace<PositionComponent>(mouse, mouseTilePosition);
}

void drft::system::MouseStateSystem::updateMouseContextualState(entt::entity mouse) const
{
	auto player = getPlayerConstHandle(_registry);
	auto& grid = _registry.ctx().get<spatial::WorldGrid>();
	auto mouseTilePosition = getMouseTilePosition(_registry);
	auto& mouseCursor = _registry.get<MouseCursorComponent>(mouse);
	mouseCursor.actionState = MouseContextualActionState::Move;

	// Whats under the mouse position?
	const auto& entities = grid.entitiesAt(mouseTilePosition);
	for (auto&& entity : entities)
	{
		auto handle = entt::const_handle{ _registry, entity };
		if (FactionSystem::resolveRelationship(player, handle) == Relationship::Hostile)
		{
			mouseCursor.actionState = MouseContextualActionState::Attack;
			return;
		}
		if (handle.any_of<PhysicalBlockingComponent>())
		{
			mouseCursor.actionState = MouseContextualActionState::Attack;
			return;
		}

		if (handle.any_of<InteractableComponent>())
		{
			mouseCursor.actionState = MouseContextualActionState::Interact;
			continue;
		}
	}
}
