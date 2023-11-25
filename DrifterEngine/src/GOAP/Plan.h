#pragma once
#include "IAction.h"
#include "Actions/AiActionTypes.h"

namespace drft::goap
{
	using Plan = std::deque<AiAction>;

	[[nodiscard]]
	std::optional<Plan> plan(const goap::WorldState& blackboard, const std::unordered_set<AiAction>& actions, const WorldState& goal);
}


