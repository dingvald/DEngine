#pragma once

namespace drft::events
{
	struct ItemBreakEvent
	{
		unsigned long itemID = 0u;
		entt::entity owner = entt::null;
	};
}