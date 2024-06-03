#include <pch.h>
#include "MoveActionSystem.h"
#include "Spatial/WorldGrid.h"
#include "Spatial/Conversions.h"

#include "Components/Components.h"
#include "Components/CollisionComponent.h"
#include "Components/Actions/MoveAction.h"
#include "Components/MaterialComponent.h"
#include "Components/PositionComponent.h"
#include "Components/StaminaComponent.h"

#include "Systems/Helpers/SpendActionPoints.h"
#include "Utility/EntityHelpers.h"

void drft::system::MoveActionSystem::init()
{
	_registry->on_construct<PerformMoveAction>().connect<&MoveActionSystem::onPerformMoveAction>(this);
	_registry->on_construct<TryMoveAction>().connect<&MoveActionSystem::onTryMoveAction>(this);
	_registry->on_construct<DoMoveAction>().connect<&MoveActionSystem::onDoMoveAction>(this);
}

void drft::system::MoveActionSystem::onUpdateEnd()
{
	_registry->clear<PerformMoveAction>();
	_registry->clear<TryMoveAction>();
	_registry->clear<DoMoveAction>();
}

void drft::system::MoveActionSystem::onPerformMoveAction(entt::registry& registry, entt::entity entity) const
{
	const auto& performMove = registry.get<PerformMoveAction>(entity);
	if (performMove.direction == sf::Vector2i{ 0,0 })
	{
		_registry->emplace_or_replace<component::action::Wait>(entity);
	}
	else
	{
		registry.emplace_or_replace<TryMoveAction>(entity, performMove.direction);
	}
}

void drft::system::MoveActionSystem::onTryMoveAction(entt::registry& registry, entt::entity entity) const
{
	const auto& grid = _registry->ctx().get<spatial::WorldGrid&>();
	const auto& tryMove = registry.get<TryMoveAction>(entity);
	const auto& positionComponent = registry.get<PositionComponent>(entity);

	sf::Vector2i targetPosition = positionComponent.position + tryMove.direction;
	auto checkForBlockers = [this](entt::entity entity) -> bool
		{
			if (auto material = _registry->try_get<MaterialComponent>(entity))
			{
				return material->blocks;
			}
			return false;
		};
	auto blockers = grid.entitiesAt(targetPosition, checkForBlockers);

	if (blockers.empty())
	{
		_registry->emplace_or_replace<DoMoveAction>(entity);
	}
	else
	{
		_registry->emplace_or_replace<CollisionComponent>(entity, tryMove.direction, std::move(blockers));
	}
}

void drft::system::MoveActionSystem::onDoMoveAction(entt::registry& registry, entt::entity entity) const
{
	if (!registry.all_of<PositionComponent>(entity)) return;

	const auto& doMoveAction = registry.get<TryMoveAction>(entity);
	registry.patch<PositionComponent>(entity,
		[doMoveAction](PositionComponent& positionComponent)
		{
			positionComponent.position += doMoveAction.direction;
		});
	spendActionPoints(BASE_ACTION_COST, ActionType::Move, { registry, entity });
}
