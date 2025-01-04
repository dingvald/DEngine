#pragma once
#include <AI/Utility/Impl/UtilityAISharedTypes.h>

template<typename EntityType>
struct UtilityActionTargetPair
{
	UtilityActionID action;
	EntityType target;
};