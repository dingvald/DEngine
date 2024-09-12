#include "pch.h"
#include "SpotHostileAction.h"
#include "Components/Components.h"
#include "Components/PositionComponent.h"
#include "Components/AIComponent.h"
#include "Events/SendFloatingMessageEvent.h"
#include "Utility/EntityHelpers.h"
#include <Spatial/Conversions.h>
#include "Spatial/Helpers.h"
#include "Systems/Gameplay/FactionSystem.h"

drft::goap::SpotHostileAction::SpotHostileAction()
{
	addPrecondition(visually_sense_hostile, 2);
	addEffect(spotted_hostile, true);
}

drft::goap::ActionResult drft::goap::SpotHostileAction::perform(entt::handle agent) const
{
	auto& ai = getAI(agent);
	auto& dispatcher = agent.registry()->ctx().get<entt::dispatcher&>();
	dispatcher.trigger(events::SendFloatingMessageEvent{
		.message = "!",
		.color = sf::Color::Yellow,
		.tracksEntity = agent.entity(),
		.position = spatial::toXY(spatial::toFloatSpace(agent.get<PositionComponent>().tile)),
		.velocity = {0,0},
		.isScreenSpace = false,
		.ttl = 80
		});

	ai.blackboard.merge(effects());
	agent.emplace_or_replace<component::action::Wait>();
	return ActionResult::Complete;
}

int drft::goap::SpotHostileAction::cost() const
{
	return 1;
}

bool drft::goap::SpotHostileAction::isInRange(entt::handle agent) const
{
	return true;
}
