#pragma once
#include <EnTT/entt.hpp>

namespace drft::events
{
	struct ChunkSourceTransferRequestEvent
	{
		entt::id_type sourceId;
	};
}