#include <pch.h>
#include "MovementSystem.h"
#include "Spatial/WorldGrid.h"
#include "Spatial/Conversions.h"
#include "Components/Components.h"
#include "Components/Tags.h"
#include "Systems/Helpers/SpendActionPoints.h"
#include "Utility/EntityHelpers.h"

void drft::system::MovementSystem::init()
{
}

void drft::system::MovementSystem::update(const float dt)
{
	const auto& grid = _registry->ctx().get<spatial::WorldGrid&>();
	auto moveView = _registry->view<component::action::Move, component::Position>();
	for (auto [entity, move, pos] : moveView.each())
	{
		if (move.direction == sf::Vector2i{ 0,0 })
		{
			_registry->emplace_or_replace<component::action::Wait>(entity);
			_registry->remove<component::action::Move>(entity);
			continue;
		}

		sf::Vector2i targetPosition = pos.position + move.direction;
		const auto blockers = grid.entitiesAt(targetPosition,
			[this](entt::entity entity) -> bool
			{
				if (auto physical = _registry->try_get<component::Physical>(entity))
				{
					if (physical->blocks)
					{
						return true;
					}
				}
				return false;
			});

		if (blockers.empty())
		{
			_registry->patch<component::Position>(entity,
				[targetPosition](component::Position& pos)
				{
					pos.position = targetPosition;
				});
			if (_registry->all_of<component::Stamina>(entity))
			{
				_registry->emplace_or_replace<component::action::ConsumeStamina>(entity, -0.25f);
			}
			spendActionPoints(BASE_ACTION_COST, ActionType::Move, { *_registry, entity });
		}
		else
		{
			_registry->emplace_or_replace<component::action::LaunchAttack>(entity, move.direction);
		}
	}
}

void drft::system::MovementSystem::onUpdateEnd()
{
	_registry->clear<component::action::Move>();
}
