#include "pch.h"
#include "InvestigateHostileAction.h"
#include "Components/Components.h"
#include "Systems/Gameplay/FactionSystem.h"
#include "Spatial/Helpers.h"
#include "Events/SendFloatingMessageEvent.h"

drft::goap::InvestigateHostileAction::InvestigateHostileAction()
{
    addPrecondition(visually_sense_hostile, 1);
    addEffect(visually_sense_hostile, 2);
}

std::optional<sf::Vector2i> drft::goap::InvestigateHostileAction::trySetTarget(entt::handle agent) const
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

drft::goap::ActionResult drft::goap::InvestigateHostileAction::perform(entt::handle agent) const
{
	auto& dispatcher = agent.registry()->ctx().get<entt::dispatcher&>();
	dispatcher.trigger(events::SendFloatingMessageEvent{
		.message = "?",
		.color = sf::Color::Yellow,
		.position = agent.get<component::Position>().position,
		.velocity = {0,0},
		.isScreenSpace = false,
		.ttl = 120
		});
    return ActionResult();
}

int drft::goap::InvestigateHostileAction::cost() const
{
    return 1;
}

bool drft::goap::InvestigateHostileAction::isInRange(entt::handle agent) const
{
	const auto& ai = getAI(agent);
	if (ai.blackboard.contains({ {visually_sense_hostile, 2 } }))
	{
		return true;
	}
    return false;
}
