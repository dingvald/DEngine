#include <pch.h>
#include "MovementSystem.h"
#include "Spatial/WorldGrid.h"
#include "Spatial/Conversions.h"
#include "Components/Components.h"
#include "Components/Tags.h"
#include "Utility/EntityHelpers.h"

void drft::system::MovementSystem::init()
{
}

void drft::system::MovementSystem::update(const float dt)
{
	auto moveView = registry->view<component::action::Move, component::tag::Active>();
	for (auto [entity, move] : moveView.each())
	{
		if (!registry->all_of<component::Position>(entity)) {
			registry->remove<component::action::Move>(entity);
			continue;
		}
		if (move.direction == sf::Vector2i{ 0,0 })
		{
			registry->emplace_or_replace<component::action::Wait>(entity);
			registry->remove<component::action::Move>(entity);
			continue;
		}

		const auto& grid = registry->ctx().get<spatial::WorldGrid&>();
		auto& posComp = registry->get<component::Position>(entity);
		sf::Vector2i targetPosition = spatial::toTileSpace(posComp.position) + move.direction;

		const auto blockers = grid.entitiesAt(targetPosition, spatial::Layer::Blocking);
		const auto props = grid.entitiesAt(targetPosition, spatial::Layer::Prop);
		if (blockers.empty() && props.empty())
		{
			registry->patch<component::Position>(entity,
				[&targetPosition](component::Position& pos)
				{
					pos.position = spatial::toWorldSpace(targetPosition);
				}
			);

			const int actionCost = util::getActionCost({ *registry, entity }, 100, util::ActionType::Move);
			registry->emplace_or_replace<component::action::SpendPoints>(entity, actionCost);
		}
		else
		{
			registry->emplace_or_replace<component::action::LaunchAttack>(entity, move.direction);
		}

		registry->remove<component::action::Move>(entity);
	}

	auto waitView = registry->view<component::action::Wait, component::tag::Active>();
	for (auto entity : waitView)
	{
		registry->emplace_or_replace<component::action::SpendPoints>(entity, 100);
		registry->remove<component::action::Wait>(entity);
	}
}
