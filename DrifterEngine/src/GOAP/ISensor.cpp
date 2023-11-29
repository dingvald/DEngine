#include "pch.h"
#include "ISensor.h"
#include "Components/Components.h"

void drft::goap::ISensor::sense(entt::handle agent, std::function<bool(entt::const_handle, sf::Vector2i)> checker) const
{
	const auto isInSurroundings = checkAndFillSurroundings(agent, checker);
	const auto isInMemory = checkMemory(agent);
	mergeResults(isInSurroundings || isInMemory, getAI(agent));
}

void drft::goap::ISensor::mergeResults(bool senseWasSuccess, component::AI& ai) const
{
	if (senseWasSuccess)
	{
		ai.blackboard.merge(stateAfterSuccess());
	}
	else
	{
		ai.blackboard.merge(stateAfterFailure());
	}
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
