#include "pch.h"
#include "EscapeHostileGoal.h"
#include "Components/Components.h"

drft::goap::EscapeHostileGoal::EscapeHostileGoal()
{
	addDesiredState(escape_hostile, true);
}

float drft::goap::EscapeHostileGoal::utility(entt::const_handle agent) const
{
	if (auto health = agent.try_get<component::Health>())
	{
		return (1.0f - (health->current / health->max));
	}
	return 0.0f;
}
