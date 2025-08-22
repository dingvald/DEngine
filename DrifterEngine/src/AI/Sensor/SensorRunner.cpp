#include "pch.h"
#include "SensorRunner.h"

void SensorRunner::runSensors(entt::handle agent)
{
	for (auto& sensor : _sensors)
	{
		if (!sensor->canUseSensor(agent)) continue;

		sensor->sense(agent);
	}
}
