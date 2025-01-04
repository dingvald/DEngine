#include "pch.h"
#include "ExploreGoal.h"

drft::goap::ExploreGoal::ExploreGoal()
{
	addDesiredState(look_busy, true);
}

float drft::goap::ExploreGoal::utility(entt::const_handle agent) const
{
	return 0.1f;
}
