#include "pch.h"
#include "AttackHostileAction.h"
#include "Components/Components.h"
#include "Spatial/Helpers.h"
#include "Systems/Gameplay/FactionSystem.h"

drft::goap::AttackHostileAction::AttackHostileAction()
{
	addPrecondition(sees_hostile, true);

	addEffect(escape_hostile, true);
}

std::optional<sf::Vector2i> drft::goap::AttackHostileAction::trySetTarget(entt::handle agent) const
{
	auto& ai = getAI(agent);
	std::optional<sf::Vector2i> result = {};
	if (ai.target == entt::null || !agent.registry()->valid(ai.target))
	{
		for (auto&& [sensorType, memoryMap] : ai.surroundings)
		{
			for (auto&& [entity, _] : memoryMap)
			{
				auto otherHandle = entt::const_handle{ *agent.registry(), entity };
				if (!otherHandle.all_of<component::Position>()) continue;
				if (system::FactionSystem::resolveRelationship(agent, otherHandle) != system::Relationship::Hostile) continue;

				auto& myPos = agent.get<component::Position>();
				auto& otherPos = otherHandle.get<component::Position>();
				if (ai.target != entt::null)
				{
					auto& currentTargetPos = agent.registry()->get<component::Position>(ai.target);
					const int currentTargetDistance = spatial::distance(myPos.position, currentTargetPos.position);
					const int newTargetDistance = spatial::distance(myPos.position, otherPos.position);
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