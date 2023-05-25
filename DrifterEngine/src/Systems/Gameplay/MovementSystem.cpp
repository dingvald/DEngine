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
	const auto& grid = registry->ctx().get<spatial::WorldGrid&>();
	auto moveView = registry->view<component::action::Move, component::Position, component::tag::Active>();
	for (auto [entity, move, pos] : moveView.each())
	{
		if (move.direction == sf::Vector2i{ 0,0 })
		{
			registry->emplace_or_replace<component::action::Wait>(entity);
			registry->remove<component::action::Move>(entity);
			continue;
		}

		sf::Vector2i targetPosition = spatial::toTileSpace(pos.position) + move.direction;
		const auto blockers = grid.entitiesAt(targetPosition,
			[this](entt::entity entity) -> bool
			{
				if (auto physical = registry->try_get<component::Physical>(entity))
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
			registry->patch<component::Position>(entity,
				[&targetPosition](component::Position& pos)
				{
					pos.position = spatial::toWorldSpace(targetPosition);
				});
			registry->emplace_or_replace<component::action::ConsumeStamina>(entity, -1);
			spendActionPoints(*registry, entity, ActionType::Move);
		}
		else
		{
			registry->emplace_or_replace<component::action::LaunchAttack>(entity, move.direction);
		}
	}
}

void drft::system::MovementSystem::onUpdateEnd()
{
	registry->clear<component::action::Move>();
}
