#pragma once
#include <EnTT/entt.hpp>

namespace drft::events
{
	struct ChunkSourceTransferCompleteEvent
	{
		entt::id_type newSourceId;
	};
}