#include "pch.h"
#include "HostileSensor.h"
#include "Components/Components.h"
#include "Components/Tags.h"
#include "Systems/Gameplay/FactionSystem.h"
#include "Events/SendFloatingMessageEvent.h"
#include "Utility/EntityHelpers.h"
#include "Spatial/Helpers.h"
#include "GOAP/WorldStateTypes.h"

drft::goap::SensorType drft::goap::HostileSensor::getType() const
{
	return SensorType::Visual;
}

int drft::goap::HostileSensor::checkAndFillSurroundings(entt::handle agent, std::function<bool(entt::const_handle, sf::Vector2i)> checker) const
{
	auto& ai = getAI(agent);
	auto& myPos = agent.get<component::Position>();
	bool success = false;
	auto view = agent.registry()->view<component::Position, component::Faction, component::tag::Active>();
	int magnitude = 0;
	for (const auto& [entity, pos, faction] : view.each())
	{
		if (system::FactionSystem::resolveRelationship(agent, { *agent.registry(), entity }) != system::Relationship::Hostile) continue;
		if ((spatial::distance(myPos.position, pos.position) <= (ai.sightRange + 1)) && checker(agent, pos.position))
		{
			ai.surroundings[getType()][entity] = std::max(ai.surroundings[getType()][entity], SensorMemory.at(getType()));
			if (spatial::distance(myPos.position, pos.position) <= (ai.sightRange - 1))
			{
				magnitude = 2;
			}
			else
			{
				magnitude = 1;
			}
		}
	}
	return magnitude;
}

int drft::goap::HostileSensor::checkMemory(entt::const_handle agent) const
{
	if (!getAI(agent).surroundings.contains(getType())) return false;
	for (auto&& [entity, _] : getAI(agent).surroundings.at(getType()))
	{
		if (system::FactionSystem::resolveRelationship(agent, { *agent.registry(), entity }) == system::Relationship::Hostile)
		{
			return 1;
		}
	}
	return 0;
}

std::vector<drft::goap::WorldStateType> drft::goap::HostileSensor::stateTypesSensed() const
{
	return { visually_sense_hostile };
}

