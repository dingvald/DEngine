#include "pch.h"
#include "ISensor.h"
#include "Components/Components.h"

void drft::goap::ISensor::sense(entt::handle agent, std::function<bool(entt::const_handle, sf::Vector2i)> checker) const
{
	const auto stateFromSurroundings = checkAndFillSurroundings(agent, checker);
	const auto stateFromMemory = checkMemory(agent);
	mergeResults(stateFromSurroundings, stateFromMemory, getAI(agent));
}

void drft::goap::ISensor::mergeResults(const WorldState& fromSurroundings, const WorldState& fromMemory, component::AI& ai) const
{
	ai.blackboard.merge(fromSurroundings);
	ai.blackboard.mergeIfGreater(fromMemory);
}

component::AI& drft::goap::ISensor::getAI(entt::handle agent) const
{
	if (auto ai = agent.try_get<component::AI>()) return *ai;
	throw std::exception("Agent does not have AI component.");
}

const component::AI& drft::goap::ISensor::getAI(entt::const_handle agent) const
{
	if (auto ai = agent.try_get<component::AI>()) return *ai;
	throw std::exception("Agent does not have AI component.");
}
