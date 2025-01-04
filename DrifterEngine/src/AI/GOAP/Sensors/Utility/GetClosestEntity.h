#pragma once
#include "AI/GOAP/SensorTypes.h"

namespace drft::goap
{
	// Gets the closest entity in the agent's surroundings that matches the filter function.
	entt::const_handle getClosestEntity(entt::const_handle agent, std::vector<SensorType> sensorTypes, std::function<bool(entt::const_handle, entt::const_handle)> filter);
}