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
		virtual void sense(entt::handle agent, std::function<bool(entt::const_handle, sf::Vector2i)> checker) const = 0;

	protected:
		// The world state after successfully sensing the agent's surroundings.
		virtual WorldState stateAfterSuccess() const = 0;
		// The world state after failing to sense the agent's surroundings.
		virtual WorldState stateAfterFailure() const = 0;
		component::AI& getAI(entt::handle agent) const;
	};
}

