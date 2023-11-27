#pragma once
#include "GOAP/ISensor.h"

namespace drft::goap
{
	class HostileSensor : public ISensor
	{
		// Inherited via ISensor
		virtual SensorType getType() const override;
		virtual void sense(entt::handle agent, std::function<bool(entt::const_handle, sf::Vector2i)> checker) const override;
		virtual WorldState stateAfterSuccess() const override;
		virtual WorldState stateAfterFailure() const override;
	};
}

