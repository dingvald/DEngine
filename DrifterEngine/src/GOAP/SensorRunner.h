#pragma once
#include "ISensor.h"
#include "SensorTypes.h"

namespace drft::goap
{
	class SensorRunner
	{
	public:
		using CheckerFxn = std::function<bool(entt::const_handle, sf::Vector2i)>;
		void decayMemory(std::unordered_map<drft::goap::SensorType, std::unordered_map<entt::entity, int>>& surroundings) const;
		void registerSensor(std::unique_ptr<goap::ISensor> sensor);
		void registerChecker(CheckerFxn checker, SensorType type);
		void runSensors(entt::handle agent);

	private:
		std::vector<std::unique_ptr<ISensor>> _sensors;
		std::unordered_map<SensorType, CheckerFxn> _checkers;
	};
}


