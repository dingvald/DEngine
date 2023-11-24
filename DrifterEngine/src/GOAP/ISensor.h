#pragma once
#include "WorldState.h"

namespace drft::goap
{
	class ISensor
	{
	public:
		virtual WorldState sense(entt::const_handle agent, const std::vector<entt::entity>& surroundings = {}) const = 0;
	};
}

