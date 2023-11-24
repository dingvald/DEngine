#include "pch.h"
#include "HostileSensor.h"
#include "Components/Components.h"
#include "Systems/Gameplay/FactionSystem.h"
#include "Events/SendFloatingMessageEvent.h"
#include "Utility/EntityHelpers.h"

drft::goap::WorldState drft::goap::HostileSensor::sense(entt::const_handle agent, const std::vector<entt::entity>& surroundings) const
{
	for (auto&& entity : surroundings)
	{
		auto otherHandle = entt::const_handle{ *agent.registry(), entity };
		if (system::FactionSystem::resolveRelationship(agent, otherHandle) == system::Relationship::Hostile)
		{
			auto position = otherHandle.get<component::Position>().position;
			return WorldState{ {"near_hostile", true} };
		}
	}
	return WorldState{ {"near_hostile", false} };
}
