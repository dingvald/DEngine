#include "pch.h"
#include "HostileSensor.h"

#include "Components/Components.h"
#include "Components/AIComponent.h"
#include "Components/PositionComponent.h"
#include "Components/FactionComponent.h"
#include "Components/Tags.h"

#include "Systems/Gameplay/FactionSystem.h"
#include "Events/SendFloatingMessageEvent.h"
#include "Utility/EntityHelpers.h"
#include "Spatial/Helpers.h"
#include "AI/GOAP/WorldStateTypes.h"
#include "Utility/IsHostile.h"

drft::goap::SensorType drft::goap::HostileSensor::getType() const
{
	return SensorType::Visual;
}

drft::goap::WorldState drft::goap::HostileSensor::checkAndFillSurroundings(entt::handle agent, CheckerFxn checker) const
{
	auto& ai = getAI(agent);
	auto& myPos = agent.get<PositionComponent>();
	bool success = false;
	auto view = agent.registry()->view<PositionComponent, FactionComponent, component::tag::Active>();
	int magnitude = 0;
	for (const auto& [entity, pos, faction] : view.each())
	{
		if (!filter::isHostile(agent, { *agent.registry(), entity })) continue;
		if (spatial::isWithinRadius3d(myPos.tile, pos.tile, ai.sightRange) && checker(agent, pos.tile))
		{
			magnitude = std::max(magnitude, 1);
			// Refresh memory
			ai.surroundings[getType()][entity] = std::max(ai.surroundings[getType()][entity], SensorMemory.at(getType()));
			if (spatial::isWithinRadius3d(myPos.tile, pos.tile, ai.sightRange - 1))
			{
				magnitude = std::max(magnitude, 2);
			}
		}
	}
	return WorldState{ {visually_sense_hostile, magnitude} };
}

drft::goap::WorldState drft::goap::HostileSensor::checkMemory(entt::const_handle agent) const
{
	if (!getAI(agent).surroundings.contains(getType())) return {};

	for (auto&& [entity, _] : getAI(agent).surroundings.at(getType()))
	{
		if (system::FactionSystem::resolveRelationship(agent, { *agent.registry(), entity }) == system::Relationship::Hostile)
		{
			return WorldState{ {visually_sense_hostile, 1} };
		}
	}
	return {};
}
