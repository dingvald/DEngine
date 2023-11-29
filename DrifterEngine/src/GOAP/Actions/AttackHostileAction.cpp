#include "pch.h"
#include "AttackHostileAction.h"
#include "Components/Components.h"
#include "Spatial/Helpers.h"
#include "Systems/Gameplay/FactionSystem.h"

drft::goap::AttackHostileAction::AttackHostileAction()
{
	addPrecondition(sees_hostile, true);

	addEffect(sees_hostile, false);
	addEffect(target_dead, true); 
}

std::optional<sf::Vector2i> drft::goap::AttackHostileAction::trySetTarget(entt::handle agent) const
{
	auto& ai = getAI(agent);
	std::optional<sf::Vector2i> result = {};
	if (ai.target == entt::null || !agent.registry()->valid(ai.target))
	{
		for (auto entity : ai.entitiesOfInterest)
		{
			if (system::FactionSystem::resolveRelationship(agent, { *agent.registry(), entity }) != system::Relationship::Hostile)
			{
				continue;
			}

			if (auto targetPos = agent.registry()->try_get<component::Position>(entity))
			{
				auto& myPos = agent.get<component::Position>();
				if (ai.target != entt::null)
				{
					auto& currentTargetPos = agent.registry()->get<component::Position>(ai.target);
					const int currentTargetDistance = spatial::distance(myPos.position, currentTargetPos.position);
					const int newTargetDistance = spatial::distance(myPos.position, targetPos->position);
					if (newTargetDistance < currentTargetDistance)
					{
						ai.target = entity;
					}
				}
				else
				{
					ai.target = entity;
				}
			}
		}
	}
	if (ai.target != entt::null && agent.registry()->valid(ai.target))
	{
		result = agent.registry()->get<component::Position>(ai.target).position;
	}

	return result;
}

drft::goap::ActionResult drft::goap::AttackHostileAction::perform(entt::handle agent) const
{
	auto& ai = getAI(agent);
	if (ai.target == entt::null)
	{
		return ActionResult::Failed;
	}

	if (auto targetPos = agent.registry()->try_get<component::Position>(ai.target))
	{
		const auto& pos = agent.get<component::Position>();
		sf::Vector2i targetDirection = targetPos->position - pos.position;
		agent.emplace_or_replace<component::action::LaunchAttack>(targetDirection);
		std::cout << "Success!" << std::endl;
		return ActionResult::Continue;
	}

	std::cout << "Failed: Target does not have a position" << std::endl;
	return ActionResult::Failed;
}

int drft::goap::AttackHostileAction::cost() const
{
	return 2;
}

bool drft::goap::AttackHostileAction::isInRange(entt::handle agent) const
{
	auto& ai = getAI(agent);
	if (ai.target == entt::null)
	{
		return false;
	}

	auto& pos = agent.get<component::Position>();
	auto& targetPos = agent.registry()->get<component::Position>(ai.target);
	if (spatial::distance(pos.position, targetPos.position) <= 1)
	{
		return true;
	}
	return false;
}