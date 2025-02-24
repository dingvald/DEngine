#pragma once
#include <States/StateIdentifiers.h>

namespace drft::events
{
	struct RequestStateStackPush
	{
		States stateID = States::None;
	};
}