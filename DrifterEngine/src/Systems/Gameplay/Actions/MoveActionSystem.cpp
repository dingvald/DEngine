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
	_registry.on_construct<MoveAction>().connect<&MoveActionSystem::onMoveActionAdded>(this);
}

void drft::system::MoveActionSystem::onUpdateLate(const float dt)
{
	auto view = _registry.view<PositionComponent, MoveAction>();
	for (auto&& [entity, position, move] : view.each())
	{
		processMoveAction(entity, move);
	}
}

void drft::system::MoveActionSystem::onUpdateEnd()
{
	_registry.clear<MoveAction>();
}

void drft::system::MoveActionSystem::onMoveActionAdded(entt::registry& registry, entt::entity entity) const
{
	const auto& grid = _registry.ctx().get<spatial::WorldGrid&>();
	const auto& positionComponent = registry.get<PositionComponent>(entity);
	auto& moveAction = registry.get<MoveAction>(entity);

	sf::Vector2i targetPosition = positionComponent.position + moveAction.direction;
	auto checkForBlockers = [this](entt::entity entity) -> bool
		{
			if (auto material = _registry.try_get<MaterialComponent>(entity))
			{
				return material->blocks;
			}
			return false;
		};
	auto blockers = grid.entitiesAt(targetPosition, checkForBlockers);

	if (!blockers.empty())
	{
		_registry.emplace_or_replace<CollisionComponent>(entity, moveAction.direction, std::move(blockers));
		moveAction.direction = sf::Vector2i{ 0,0 };
	}
}

void drft::system::MoveActionSystem::processMoveAction(entt::entity entity, MoveAction& action) const
{
	if (action.direction == sf::Vector2i{ 0,0 }) return;

	_registry.patch<PositionComponent>(entity,
		[&action](PositionComponent& positionComponent)
		{
			positionComponent.position += action.direction;
		});
	spendActionPoints(BASE_ACTION_COST, ActionType::Move, { _registry, entity });
}
