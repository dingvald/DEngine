#include "pch.h"
#include "AttackHostileAction.h"

#include "Components/Actions/MeleeAttackAction.h"
#include "Components/Actions/MoveAction.h"
#include "Components/AIComponent.h"
#include "Components/PositionComponent.h"

#include "Spatial/Helpers.h"
#include "Systems/Gameplay/FactionSystem.h"
#include "GOAP/Sensors/Utility/GetClosestEntity.h"
#include "GOAP/Sensors/Utility/IsHostile.h"

constexpr float MELEE_RANGE = 1.5f; // Close enough to sqrt(2)

drft::goap::AttackHostileAction::AttackHostileAction()
{
	addPrecondition(spotted_hostile, true);

	addEffect(kill_hostile, true);
}

std::optional<sf::Vector2i> drft::goap::AttackHostileAction::trySetTarget(entt::handle agent) const
{
	auto& ai = getAI(agent);
	std::optional<sf::Vector2i> result = {};
	if (ai.target == entt::null || !agent.registry()->valid(ai.target))
	{
		auto closestEntityHandle = getClosestEntity(agent, { SensorType::Visual }, filter::isHostile);
		ai.target = closestEntityHandle.entity();
	}
	if (ai.target != entt::null && agent.registry()->valid(ai.target))
	{
		result = agent.registry()->get<PositionComponent>(ai.target).position;
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
		sf::Vector2i targetDirection = targetPos->position - pos.position;
		agent.emplace_or_replace<MeleeAttackAction>(targetDirection, std::vector<entt::entity>{ai.target});
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
		return ( spatial::distance(pos.position, targetPos->position) <= MELEE_RANGE );
	}
	
	return false;
}