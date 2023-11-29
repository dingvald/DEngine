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
		virtual bool checkAndFillSurroundings(entt::handle agent, std::function<bool(entt::const_handle, sf::Vector2i)> checker) const = 0;
		// Returns true if the agent's memory of the sensor's type contains an entity .
		virtual bool checkMemory(entt::const_handle agent) const = 0;
		// Merges the results of the sense attempt.
		void mergeResults(bool senseWasSuccess, component::AI& ai) const;
		// The world state after successfully sensing the agent's surroundings.
		virtual WorldState stateAfterSuccess() const = 0;
		// The world state after failing to sense the agent's surroundings.
		virtual WorldState stateAfterFailure() const = 0;
		component::AI& getAI(entt::handle agent) const;
		const component::AI& getAI(entt::const_handle agent) const;
	};
}

