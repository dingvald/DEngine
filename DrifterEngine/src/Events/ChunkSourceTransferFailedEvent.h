#pragma once
#include <EnTT/entt.hpp>

namespace drft::events
{
	struct ChunkSourceTransferFailedEvent
	{
		entt::id_type sourceId;
	};
}