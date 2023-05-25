#pragma once
#include "EnTT/entt.h"

namespace drft::events
{
	struct TurnEndEvent
	{
		entt::entity entity;
	};
}