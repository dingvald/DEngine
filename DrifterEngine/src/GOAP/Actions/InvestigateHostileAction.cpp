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

drft::goap::ActionResult drft::goap::InvestigateHostileAction::perform(entt::handle agent) const
{
	auto& ai = getAI(agent);

	switch (ai.blackboard[action_counter])
		case 0:
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
		agent.emplace_or_replace<component::action::Wait>();
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
