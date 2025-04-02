#pragma once
#include <EnTT/entt.hpp>

namespace drft::events
{
	struct ChunkSourceTransferStartedEvent
	{
		entt::id_type oldSourceId;
		entt::id_type newSourceId;
	};
}