#pragma once
#include <EnTT/entt.hpp>

namespace drft::events
{
	struct ChunkSourceTransferInProgressEvent
	{
		entt::id_type oldSourceId;
		entt::id_type newSourceId;
	};
}