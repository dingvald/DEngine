#include "pch.h"
#include "AttackHostileAction.h"

#include "Components/Actions/MeleeAttackAction.h"
#include "Components/Actions/MoveAction.h"
#include "Components/AIComponent.h"
#include "Components/PositionComponent.h"

#include "Spatial/Helpers.h"
#include "Systems/Gameplay/FactionSystem.h"
#include "AI/GOAP/Sensors/Utility/GetClosestEntity.h"
#include "AI/GOAP/Sensors/Utility/IsHostile.h"

constexpr float MELEE_RANGE = 1.5f; // Close enough to sqrt(2)

drft::goap::AttackHostileAction::AttackHostileAction()
{
	addPrecondition(spotted_hostile, true);

	addEffect(kill_hostile, true);
}

std::optional<drft::TilePosition> drft::goap::AttackHostileAction::trySetTarget(entt::handle agent) const
{
	auto& ai = getAI(agent);
	std::optional<TilePosition> result = {};
	if (ai.target == entt::null || !agent.registry()->valid(ai.target))
	{
		auto closestEntityHandle = getClosestEntity(agent, { SensorType::Visual }, filter::isHostile);
		ai.target = closestEntityHandle.entity();
	}
	if (ai.target != entt::null && agent.registry()->valid(ai.target))
	{
		result = agent.registry()->get<PositionComponent>(ai.target).tile;
	}

	return result;
}

drft::goap::ActionResult drft::goap::AttackHostileAction::perform(entt::handle agent) const
{
	auto& ai = getAI(agent);
	if (ai.target == entt::null) return ActionResult::Failed;

	if (auto targetPos = agent.registry()->try_get<PositionComponent>(ai.target))
	{
		const auto& pos = agent.get<PositionComponent>();
		sf::Vector2i targetDirection = spatial::toXY(targetPos->tile - pos.tile);
		agent.emplace_or_replace<MeleeAttackAction>(targetDirection);
		return ActionResult::Continue;
	}
	return ActionResult::Failed;
}

int drft::goap::AttackHostileAction::cost() const
{
	return 2;
}

bool drft::goap::AttackHostileAction::isInRange(entt::handle agent) const
{
	auto& ai = getAI(agent);
	if (ai.target == entt::null || !agent.registry()->valid(ai.target))
	{
		return false;
	}

	auto& pos = agent.get<PositionComponent>();
	if (auto targetPos = agent.registry()->try_get<PositionComponent>(ai.target))
	{
		return spatial::isWithinRadius3d(pos.tile, targetPos->tile, MELEE_RANGE);
	}
	
	return false;
}