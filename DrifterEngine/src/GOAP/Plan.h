#pragma once
#include "Actions/IAction.h"
#include "Actions/AiActionTypes.h"

namespace drft::goap
{
	[[nodiscard]]
	std::optional<std::deque<AiAction>> plan(const goap::WorldState& blackboard, const std::unordered_set<AiAction>& actions, const WorldState& goal);
}


