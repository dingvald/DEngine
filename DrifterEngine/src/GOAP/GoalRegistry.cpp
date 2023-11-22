#include "pch.h"
#include "GoalRegistry.h"

drft::goap::GoalRegistry::GoalMap drft::goap::GoalRegistry::_goals = {};

void drft::goap::GoalRegistry::bind()
{
	_goals["kill_hostile"] = { {"target_dead", true} };
	_goals["avoid_hostile"] = { {"sees_hostile", false} };
}

const drft::goap::GoalRegistry::Goal& drft::goap::GoalRegistry::get(const std::string& goalName)
{
	if (!_goals.contains(goalName)) throw std::exception("Goal name does not exist.");
	return _goals.at(goalName);
}
