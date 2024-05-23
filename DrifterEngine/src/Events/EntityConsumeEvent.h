#pragma once
#include "EnTT/entt.h"

namespace drft::events
{
	struct EntityConsumeEvent
	{
		entt::entity consumer;
		entt::entity consumed;
	};
}