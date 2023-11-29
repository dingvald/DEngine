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

void drft::goap::HostileSensor::sense(entt::handle agent, std::function<bool(entt::const_handle, sf::Vector2i)> checker) const
{
	auto view = agent.registry()->view<component::Position, component::Faction, component::tag::Active>();
	auto& ai = getAI(agent);
	auto& myPos = agent.get<component::Position>();
	bool success = false;

	for (const auto& [entity, pos, faction] : view.each())
	{
		if (system::FactionSystem::resolveRelationship(agent, { *agent.registry(), entity }) != system::Relationship::Hostile) continue;
		if ((spatial::distance(myPos.position, pos.position) <= (ai.sightRange)) && checker(agent, pos.position))
		{
			success = true;
			ai.entitiesOfInterest.push_back(entity);
		}
	}

	success ? getAI(agent).blackboard.merge(stateAfterSuccess())
		: getAI(agent).blackboard.merge(stateAfterFailure());
}

drft::goap::WorldState drft::goap::HostileSensor::stateAfterSuccess() const
{
	return WorldState{
		{visually_sense_hostile, true}
	};
}

drft::goap::WorldState drft::goap::HostileSensor::stateAfterFailure() const
{
	return WorldState{
		{visually_sense_hostile, false},
		{sees_hostile, false}
	};
}


