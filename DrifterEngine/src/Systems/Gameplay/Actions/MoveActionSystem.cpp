#include <pch.h>
#include "MoveActionSystem.h"
#include "Spatial/WorldGrid.h"
#include "Spatial/Conversions.h"

#include "Components/Components.h"
#include "Components/MaterialComponent.h"
#include "Components/PositionComponent.h"
#include "Components/MotionComponent.h"
#include "Components/StaminaComponent.h"

#include "Systems/Helpers/SpendActionPoints.h"
#include "Utility/EntityHelpers.h"

void drft::system::MoveActionSystem::init()
{
	_registry->on_construct<component::action::TryMove>().connect<&MoveActionSystem::onTryMoveAction>(this);
	_registry->on_construct<component::action::DoMove>().connect<&MoveActionSystem::onDoMoveAction>(this);
}

void drft::system::MoveActionSystem::onUpdateEnd()
{
	_registry->clear<component::action::TryMove>();
	_registry->clear<component::action::DoMove>();
}

void drft::system::MoveActionSystem::onTryMoveAction(entt::registry& registry, entt::entity entity) const
{
	if (!registry.all_of<PositionComponent>(entity)) return;

	const auto& tryMoveAction = registry.get<component::action::TryMove>(entity);
	if (tryMoveAction.direction == sf::Vector2i{ 0,0 })
	{
		_registry->emplace_or_replace<component::action::Wait>(entity);
	}
	else
	{
		registry.emplace_or_replace<MotionComponent>(entity, tryMoveAction.direction);
	}
}

void drft::system::MoveActionSystem::onDoMoveAction(entt::registry& registry, entt::entity entity) const
{
	if (!registry.all_of<PositionComponent>(entity)) return;

	const auto& doMoveAction = registry.get<component::action::TryMove>(entity);
	registry.patch<PositionComponent>(entity,
		[doMoveAction](PositionComponent& positionComponent)
		{
			positionComponent.position += doMoveAction.direction;
		});
	spendActionPoints(BASE_ACTION_COST, ActionType::Move, { registry, entity });
}
