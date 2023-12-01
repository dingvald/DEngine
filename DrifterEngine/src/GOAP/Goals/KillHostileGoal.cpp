#include "pch.h"
#include "KillHostileGoal.h"

drft::goap::KillHostileGoal::KillHostileGoal()
{
    addDesiredState(escape_hostile, true);
}

float drft::goap::KillHostileGoal::utility(entt::const_handle agent) const
{
    return 0.5f;
}
