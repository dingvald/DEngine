#include "pch.h"
#include "InvestigateHostileAction.h"
#include "Components/Components.h"
#include "Systems/Gameplay/FactionSystem.h"
#include "Spatial/Helpers.h"
#include "Events/SendFloatingMessageEvent.h"
#include "GOAP/Sensors/Utility/GetClosestEntity.h"
#include "GOAP/Sensors/Utility/IsHostile.h"

drft::goap::InvestigateHostileAction::InvestigateHostileAction()
{
    addPrecondition(visually_sense_hostile, 1);
    addEffect(visually_sense_hostile, 2);
}

drft::goap::ActionResult drft::goap::InvestigateHostileAction::perform(entt::handle agent) const
{
	auto& ai = getAI(agent);
	auto& myPos = agent.get<component::Position>();
	int targetX = ai.blackboard[target_x];
	int targetY = ai.blackboard[target_y];

	switch (ai.blackboard[action_counter])
	{
	case 0:
		// Get target position and show the '?' above head
		{
			auto closestEntityHandle = getClosestEntity(agent, { SensorType::Visual }, filter::isHostile);
			if (!closestEntityHandle.valid()) return ActionResult::Failed;
			auto& pos = closestEntityHandle.get<component::Position>();
			ai.blackboard[target_x] = pos.position.x;
			ai.blackboard[target_y] = pos.position.y;
			auto& dispatcher = agent.registry()->ctx().get<entt::dispatcher&>();
			dispatcher.trigger(events::SendFloatingMessageEvent{
				.message = "?",
				.color = sf::Color::Yellow,
				.position = agent.get<component::Position>().position,
				.velocity = {0,0},
				.isScreenSpace = false,
				.ttl = 120
				});
			agent.emplace_or_replace<component::action::Wait>();
			return ActionResult::Continue;
		}
		break;
	case 1:
		{
			auto line = spatial::getIntPointsAlongLine(myPos.position, { targetX, targetY });
			auto diff = line.front() - myPos.position;
			agent.emplace_or_replace<component::action::Move>(diff);
		}
		break;
	case 2:
		{
			auto line = spatial::getIntPointsAlongLine(myPos.position, { targetX, targetY });
			auto diff = line.front() - myPos.position;
			agent.emplace_or_replace<component::action::Move>(diff);
		}
		break;
	case 3:
		{
			if (ai.blackboard[visually_sense_hostile] < 2)
			{
				return ActionResult::Failed;
			}
			else
			{
				return ActionResult::Complete;
			}
		}
		break;
	default:
		return ActionResult::Error;
	}
    return ActionResult::Continue;
}

int drft::goap::InvestigateHostileAction::cost() const
{
    return 1;
}

bool drft::goap::InvestigateHostileAction::isInRange(entt::handle agent) const
{
	return true;
}
