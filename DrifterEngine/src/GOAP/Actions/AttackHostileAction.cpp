#include "pch.h"
#include "AttackHostileAction.h"
#include "Components/Components.h"
#include "Components/AIComponent.h"
#include "Components/PositionComponent.h"
#include "Spatial/Helpers.h"
#include "Systems/Gameplay/FactionSystem.h"
#include "GOAP/Sensors/Utility/GetClosestEntity.h"
#include "GOAP/Sensors/Utility/IsHostile.h"

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
		agent.emplace_or_replace<component::action::LaunchAttack>(targetDirection);
		std::cout << "Success!" << std::endl;
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
		if (spatial::distance(pos.position, targetPos->position) <= 1)
		{
			return true;
		}
	}
	
	return false;
}