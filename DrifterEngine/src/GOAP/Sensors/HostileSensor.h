#pragma once
#include "GOAP/ISensor.h"

namespace drft::goap
{
	class HostileSensor : public ISensor
	{
		// Inherited via ISensor
		virtual WorldState sense(entt::const_handle agent, const std::vector<entt::entity>& surroundings) const override;
	};
}

