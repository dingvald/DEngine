#pragma once
#include <AI//Utility/Impl/UtilityAISharedTypes.h>


template<typename EntityType>
class IUtilityInputProvider
{
public:
	virtual float getInput(UtilityInputID inputID, EntityType actor, EntityType target) const = 0;
};