#pragma once

namespace drft::events
{
	struct ConsumeEntityEvent
	{
		entt::entity consumer;
		entt::entity consumed;
	};
}