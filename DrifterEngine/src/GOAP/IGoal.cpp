#include "pch.h"
#include "IGoal.h"

void drft::goap::IGoal::addDesiredState(WorldStateType key, int val)
{
	_state.add(key, val);
}
