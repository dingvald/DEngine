#pragma once
#include "GOAP/ISensor.h"

namespace drft::goap
{
	class HostileSensor : public ISensor
	{
		// Inherited via ISensor
		virtual SensorType getType() const override;

		virtual int checkAndFillSurroundings(entt::handle agent, std::function<bool(entt::const_handle, sf::Vector2i)> checker) const override;
		virtual int checkMemory(entt::const_handle agent) const override;
		virtual std::vector<WorldStateType> stateTypesSensed() const override;
	};
}

