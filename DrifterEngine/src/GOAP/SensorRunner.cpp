#include "pch.h"
#include "SensorRunner.h"
#include "Spatial/Helpers.h"
#include "Spatial/WorldGrid.h"
#include "Components/Components.h"
#include "Systems/Helpers/HasLineOfSight.h"

void drft::goap::SensorRunner::registerSensor(std::unique_ptr<goap::ISensor> sensor)
{
	_sensors[sensor->getType()].emplace_back(false, std::move(sensor));
}

void drft::goap::SensorRunner::runSensors(entt::handle agent)
{
	auto& ai = agent.get<component::AI>();
	for (auto&& [type, entities] : getSensedEntities(agent))
	{
		for (auto entity : entities)
		{
			for (auto&& sensorStatus : _sensors.at(type))
			{
				if (!sensorStatus.result.keepSensing) continue;
				sensorStatus.result = sensorStatus.sensor->sense(agent, entity);
			}
		}
		for (auto& sensorStatus : _sensors.at(type))
		{
			sensorStatus.result.success
				? ai.blackboard.merge(sensorStatus.sensor->getSenseSuccess()) 
				: ai.blackboard.merge(sensorStatus.sensor->getSenseFailure());
			resetSensorResult(sensorStatus.result);
		}
	}
}

void drft::goap::SensorRunner::resetSensorResult(SenseResult& result)
{
	result.keepSensing = true;
	result.success = false;
}

std::unordered_map<drft::goap::SensorType, std::vector<entt::entity >> drft::goap::SensorRunner::getSensedEntities(entt::const_handle agent) const
{
	std::unordered_map<SensorType, std::vector<entt::entity>> result;

	// Visual
	auto& ai = agent.get<component::AI>();
	auto& pos = agent.get<component::Position>();
	auto sightRadius = spatial::getIntCircleInRadius(pos.position, ai.sightRange);
	const auto& grid = agent.registry()->ctx().get<const spatial::WorldGrid&>();
	std::vector<entt::entity> surroundings;
	for (auto&& position : sightRadius)
	{
		if (system::hasLineOfSight(agent, position))
		{
			auto entities = grid.entitiesAt(position);
			surroundings.insert(surroundings.end(), entities.begin(), entities.end());
		}
	}
	result.emplace(SensorType::Visual, surroundings);
	// End Visual

	return result;
}
