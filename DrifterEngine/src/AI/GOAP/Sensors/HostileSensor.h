#pragma once
#include "AI/GOAP/ISensor.h"

namespace drft::goap
{
	class HostileSensor : public ISensor
	{
		// Inherited via ISensor
		virtual SensorType getType() const override;
		virtual WorldState checkAndFillSurroundings(entt::handle agent, CheckerFxn checker) const override;
		virtual WorldState checkMemory(entt::const_handle agent) const override;
	};
}

