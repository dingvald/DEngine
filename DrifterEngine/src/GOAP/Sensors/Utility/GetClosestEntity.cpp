#include "pch.h"
#include "GetClosestEntity.h"
#include "Spatial/Helpers.h"
#include "Components/Components.h"

entt::const_handle drft::goap::getClosestEntity(entt::const_handle agent, std::vector<SensorType> sensorTypes, std::function<bool(entt::const_handle, entt::const_handle)> filter)
{
	if (auto ai = agent.try_get<component::AI>())
	{
		entt::entity closestEntity = entt::null;
		for (auto&& sensorType : sensorTypes)
		{
			if (!ai->surroundings.contains(sensorType)) continue;
			for (auto&& [entity, _] : ai->surroundings.at(sensorType))
			{
				auto otherHandle = entt::const_handle{ *agent.registry(), entity };
				if (!otherHandle.all_of<component::Position>()) continue;
				if (!filter(agent, otherHandle)) continue;

				auto& myPos = agent.get<component::Position>();
				auto& otherPos = otherHandle.get<component::Position>();
				if (closestEntity != entt::null)
				{
					auto& currentTargetPos = agent.registry()->get<component::Position>(closestEntity);
					const int currentTargetDistance = spatial::distance(myPos.position, currentTargetPos.position);
					const int newTargetDistance = spatial::distance(myPos.position, otherPos.position);
					if (newTargetDistance < currentTargetDistance)
					{
						closestEntity = entity;
					}
				}
				else
				{
					closestEntity = entity;
				}
			}
		}
		return entt::const_handle{ *agent.registry(), closestEntity };
	}
    return entt::const_handle{};
}
