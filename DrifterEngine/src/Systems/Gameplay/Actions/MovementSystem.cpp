#include <pch.h>
#include "MovementSystem.h"
#include "Spatial/WorldGrid.h"
#include "Spatial/Conversions.h"

#include "Components/Components.h"
#include "Components/MaterialComponent.h"
#include "Components/PositionComponent.h"
#include "Components/StaminaComponent.h"

#include "Systems/Helpers/SpendActionPoints.h"
#include "Utility/EntityHelpers.h"

void drft::system::MovementSystem::init()
{
}

void drft::system::MovementSystem::update(const float dt)
{
	const auto& grid = _registry->ctx().get<spatial::WorldGrid&>();
	auto moveView = _registry->view<component::action::Move, PositionComponent>();
	for (auto [entity, moveAction, pos] : moveView.each())
	{
		if (moveAction.direction == sf::Vector2i{ 0,0 })
		{
			_registry->emplace_or_replace<component::action::Wait>(entity);
			_registry->remove<component::action::Move>(entity);
			continue;
		}

		sf::Vector2i targetPosition = pos.position + moveAction.direction;
		auto checkForBlockers = [this](entt::entity entity) -> bool
		{
			if (auto material = _registry->try_get<MaterialComponent>(entity))
			{
				return material->blocks;
			}
			return false;
		};
		const auto blockers = grid.entitiesAt(targetPosition, checkForBlockers);

		if (blockers.empty())
		{
			_registry->patch<PositionComponent>(entity, [targetPosition](PositionComponent& pos) { pos.position = targetPosition;});
			if (_registry->all_of<StaminaComponent>(entity))
			{
				_registry->emplace_or_replace<component::action::ConsumeStamina>(entity, -0.25f);
			}
			spendActionPoints(BASE_ACTION_COST, ActionType::Move, { *_registry, entity });
		}
		else
		{
			_registry->emplace_or_replace<component::action::LaunchAttack>(entity, moveAction.direction);
		}
	}
}

void drft::system::MovementSystem::onUpdateEnd()
{
	_registry->clear<component::action::Move>();
}
