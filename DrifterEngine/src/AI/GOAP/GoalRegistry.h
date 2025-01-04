#pragma once
#include "WorldState.h"
#include "IGoal.h"

namespace drft::goap
{
	class GoalRegistry
	{
	public:
		static void bind();
		static const IGoal& get(const std::string& goalName);

	private:
		using GoalMap = std::unordered_map<std::string, std::unique_ptr<IGoal>>;
		static GoalMap _goals;
	};
}
