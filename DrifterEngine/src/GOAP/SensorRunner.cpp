#include "pch.h"
#include "SensorRunner.h"
#include "Spatial/Helpers.h"
#include "Spatial/WorldGrid.h"
#include "Components/Components.h"
#include "Systems/Helpers/HasLineOfSight.h"

void drft::goap::SensorRunner::registerSensor(std::unique_ptr<goap::ISensor> sensor)
{
	_sensors.emplace_back(std::move(sensor));
}

void drft::goap::SensorRunner::registerChecker(CheckerFxn checker, SensorType type)
{
	_checkers.emplace(type, std::move(checker));
}

void drft::goap::SensorRunner::runSensors(entt::handle agent)
{
	for (auto& sensor : _sensors)
	{
		sensor->sense(agent, _checkers.at(sensor->getType()));
	}
}
