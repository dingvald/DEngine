#pragma once
#include "WorldState.h"
#include "SensorTypes.h"

struct AIComponent;

namespace drft::goap
{
	class ISensor
	{
	public:
		virtual SensorType getType() const = 0;
		void sense(entt::handle agent, std::function<bool(entt::const_handle, sf::Vector2i)> checker) const;

	protected:
		// Returns true if the surroundings contain the entity using the checker.
		virtual WorldState checkAndFillSurroundings(entt::handle agent, std::function<bool(entt::const_handle, sf::Vector2i)> checker) const = 0;
		// Returns true if the agent's memory of the sensor's type contains an entity .
		virtual WorldState checkMemory(entt::const_handle agent) const = 0;
		// Merges the results of the sense attempt.
		void mergeResults(const WorldState& fromSurroundings, const WorldState& fromMemory, component::AI& ai) const;

		AIComponent& getAI(entt::handle agent) const;
		const AIComponent& getAI(entt::const_handle agent) const;
	};
}

