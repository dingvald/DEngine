#include "pch.h"
#include "SensorRunner.h"

void SensorRunner::runSensors(entt::const_handle agent, Blackboard& blackboard)
{
	for (auto& sensor : _sensors)
	{
		if (!sensor->canUseSensor(agent)) continue;

		sensor->sense(agent, blackboard);
	}
}
