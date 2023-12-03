#pragma once
#include "GOAP/SensorTypes.h"

namespace drft::goap
{
	bool isEntityInSurroundings(entt::const_handle agent, std::vector<SensorType> sensorTypes, std::function<bool(entt::const_handle, entt::const_handle)> filter);
}