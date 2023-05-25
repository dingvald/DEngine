#pragma once
#include "EnTT/entt.h"

namespace drft::events
{
	struct TurnStartEvent
	{
		entt::entity entity;
	};
}