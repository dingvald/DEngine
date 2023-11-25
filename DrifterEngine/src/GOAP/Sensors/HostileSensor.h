#pragma once
#include "GOAP/ISensor.h"

namespace drft::goap
{
	class HostileSensor : public ISensor
	{
		// Inherited via ISensor
		virtual SenseResult sense(entt::handle agent, entt::entity = entt::null) const override;
		virtual SensorType getType() const override;
		virtual WorldState getSenseSuccess() const override;
		virtual WorldState getSenseFailure() const override;
	};
}

