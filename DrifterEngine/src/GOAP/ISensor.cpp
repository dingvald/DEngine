#include "pch.h"
#include "ISensor.h"
#include "Components/Components.h"

void drft::goap::ISensor::sense(entt::handle agent, std::function<bool(entt::const_handle, sf::Vector2i)> checker) const
{
	const auto magnitudeFromSurroundings = checkAndFillSurroundings(agent, checker);
	const auto magnitudeFromMemory = checkMemory(agent);
	mergeResults(std::max(magnitudeFromMemory, magnitudeFromSurroundings), getAI(agent));
}

void drft::goap::ISensor::mergeResults(int magnitude, component::AI& ai) const
{
	WorldState state;
	for (auto&& type : stateTypesSensed())
	{
		state.add(type, magnitude);
	}

	ai.blackboard.merge(state);
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
