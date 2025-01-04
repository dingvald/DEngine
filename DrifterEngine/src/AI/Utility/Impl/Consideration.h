#pragma once

#include <AI/Utility/Impl/UtilityAISharedTypes.h>
#include <AI/Utility/Impl/ResponseCurve.h>

// Input type + a response curve
struct Consideration
{
	UtilityInputID inputID = NullUtilityInput;
	ResponseCurve responseCurve;
};