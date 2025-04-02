#pragma once
#include <EnTT/entt.hpp>
#include <Spatial/TilePosition.h>

namespace drft::events
{
	struct ChunkSourceTransferRequestEvent
	{
		entt::id_type sourceId;
		TilePosition position;
	};
}