#include "pch.h"
#include "GoalRegistry.h"
#include "WorldStateTypes.h"

#include "Goals/ExploreGoal.h"
#include "Goals/KillHostileGoal.h"
#include "Goals/EscapeHostileGoal.h"

drft::goap::GoalRegistry::GoalMap drft::goap::GoalRegistry::_goals = {};

void drft::goap::GoalRegistry::bind()
{
	_goals.emplace("explore", std::make_unique<ExploreGoal>());
	_goals.emplace("kill_hostile", std::make_unique<KillHostileGoal>());
	_goals.emplace("escape_hostile", std::make_unique<EscapeHostileGoal>());
}

const drft::goap::IGoal& drft::goap::GoalRegistry::get(const std::string& goalName)
{
	if (goalName == std::string())
	{
		return *_goals.at("explore");
	}
	if (!_goals.contains(goalName)) throw std::exception("Goal name does not exist.");
	return *_goals.at(goalName);
}
