#pragma once
#include "WorldState.h"

namespace drft::goap
{
	class GoalRegistry
	{
	public:
		using Goal = WorldState;
		static void bind();
		static const Goal& get(const std::string& goalName);

	private:
		using GoalMap = std::unordered_map<std::string, WorldState>;
		static GoalMap _goals;
	};
}
