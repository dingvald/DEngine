#pragma once
#include <AI/Utility/UtilityActionTargetPair.h>

template <typename EntityType>
struct ScoredUtilityAction
{
	UtilityActionTargetPair<EntityType> actionTargetPair;
	float score = 0.f;
};