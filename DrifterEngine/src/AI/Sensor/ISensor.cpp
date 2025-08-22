#include "pch.h"
#include "ISensor.h"
#include "Components/UtilityAIComponent.h"

static Blackboard EmptyBlackboard = {};

Blackboard& ISensor::getBlackboard(entt::handle agent) const
{
	if (auto ai = agent.try_get<UtilityAIComponent>())
	{
		return ai->blackboard;
	}

	EmptyBlackboard.clear();
	return EmptyBlackboard;
}
