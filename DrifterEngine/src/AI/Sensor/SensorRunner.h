#pragma once
#include "ISensor.h"
#include "SensorTypes.h"
#include <Spatial/TilePosition.h>

class Blackboard;

class SensorRunner
{
public:
    template<std::derived_from<ISensor> T>
	void registerSensor()
	{
		_sensors.emplace_back(std::make_unique<T>());
	}
	void runSensors(entt::const_handle agent, Blackboard& blackboard);

private:
	std::vector<std::unique_ptr<ISensor>> _sensors;
};
