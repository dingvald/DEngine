#include "pch.h"
#include "IsEntityInSurroundings.h"
#include "Components/AIComponent.h"

bool drft::goap::isEntityInSurroundings(entt::const_handle agent, std::vector<SensorType> sensorTypes, std::function<bool(entt::const_handle, entt::const_handle)> filter)
{
	if (auto ai = agent.try_get<AIComponent>())
	{
		for (auto&& sensorType : sensorTypes)
		{
			if (!ai->surroundings.contains(sensorType)) continue;
			for (auto&& [entity, _] : ai->surroundings.at(sensorType))
			{
				auto otherHandle = entt::const_handle{ *agent.registry(), entity };
				if (filter(agent, otherHandle)) return true;
			}
		}
	}
	return false;
}
