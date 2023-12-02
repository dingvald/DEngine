#pragma once
#include "WorldState.h"
#include "SensorTypes.h"

namespace component
{
	struct AI;
}

namespace drft::goap
{
	class ISensor
	{
	public:
		virtual SensorType getType() const = 0;
		void sense(entt::handle agent, std::function<bool(entt::const_handle, sf::Vector2i)> checker) const;

	protected:
		// Returns true if the surroundings contain the entity using the checker.
		virtual int checkAndFillSurroundings(entt::handle agent, std::function<bool(entt::const_handle, sf::Vector2i)> checker) const = 0;
		// Returns true if the agent's memory of the sensor's type contains an entity .
		virtual int checkMemory(entt::const_handle agent) const = 0;
		// Merges the results of the sense attempt.
		void mergeResults(int magnitude, component::AI& ai) const;
		// The world state types that are sensed by this sensor.
		virtual std::vector<WorldStateType> stateTypesSensed() const = 0;
		component::AI& getAI(entt::handle agent) const;
		const component::AI& getAI(entt::const_handle agent) const;
	};
}

