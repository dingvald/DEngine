#include "pch.h"
#include "HostileSensor.h"
#include "Components/Components.h"
#include "Systems/Gameplay/FactionSystem.h"
#include "Events/SendFloatingMessageEvent.h"
#include "Utility/EntityHelpers.h"

drft::goap::SenseResult drft::goap::HostileSensor::sense(entt::handle agent, entt::entity entity) const
{
	auto otherHandle = entt::const_handle{ *agent.registry(), entity };
	if (system::FactionSystem::resolveRelationship(agent, otherHandle) == system::Relationship::Hostile)
	{
		auto& ai = agent.get<component::AI>();
		if (ai.target == entt::null)
		{
			ai.target = entity;
		}
		else if (util::getDistanceBetween(agent, otherHandle) < util::getDistanceBetween(agent, {*agent.registry(), ai.target}))
		{
			ai.target = entity;
		}
		return SenseResult{ .keepSensing = true, .success = true };
	}
	return SenseResult{ .keepSensing = true, .success = false };
}

drft::goap::SensorType drft::goap::HostileSensor::getType() const
{
	return SensorType::Visual;
}

drft::goap::WorldState drft::goap::HostileSensor::getSenseSuccess() const
{
	return WorldState{
		{"sees_hostile", true},
		{"has_target", true}
	};
}

drft::goap::WorldState drft::goap::HostileSensor::getSenseFailure() const
{
	return WorldState{
		{"sees_hostile", false},
		{"has_target", false}
	};
}
