#pragma once
#include "ISensor.h"
#include "SensorTypes.h"

namespace component
{
	struct AI;
}

namespace drft::goap
{
	struct SensorStatus
	{
		SenseResult result = { .keepSensing = true, .success = false };
		std::unique_ptr<ISensor> sensor = nullptr;
	};

	class SensorRunner
	{
	public:
		void registerSensor(std::unique_ptr<goap::ISensor> sensor);
		void runSensors(entt::handle agent);

	private:
		void resetSensorResult(SenseResult& result);
		std::unordered_map<SensorType, std::vector<entt::entity>> getSensedEntities(entt::const_handle agent) const;

	private:
		mutable std::unordered_map<SensorType, std::vector<SensorStatus>> _sensors;
	};
}


