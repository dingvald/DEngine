#include "pch.h"
#include "Move.h"
#include "Spatial/WorldGrid.h"
#include "Components/Components.h"
#include "Actions/Attack.h"

using namespace drft::action;

std::unique_ptr<Action> drft::action::Move::execute(entt::registry& registry, const entt::entity actor)
{
	auto& pos = registry.get<component::Position>(actor);
	auto& grid = registry.ctx().get<spatial::WorldGrid&>();

	sf::Vector2f targetPosition = pos.position;
	targetPosition += spatial::toWorldSpace(direction);

	// TODO: add more robust collision checking to account for objects on different layers

	auto potentialBlockers = grid.entitiesAt(spatial::toTileSpace(targetPosition), spatial::toLayer(pos.depth));

	if (potentialBlockers.empty() || spatial::toLayer(pos.depth) == spatial::Layer::Item)
	{
		registry.patch<component::Position>(actor,
			[&](auto& p)
			{
				p.position = targetPosition;
				p.depth = pos.depth;
			}
		);

		return nullptr;
	}
	else
	{
		return std::make_unique<Attack>(potentialBlockers);
	}
	
	return nullptr;
}

std::unique_ptr<Action> drft::action::Move::clone() const
{
	return std::make_unique<Move>(direction);
}
