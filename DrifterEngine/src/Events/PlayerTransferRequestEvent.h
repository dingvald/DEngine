#pragma once
#include <EnTT/entt.hpp>
#include <Spatial/TilePosition.h>

namespace drft::events
{
	struct PlayerTransferRequestEvent
	{
		entt::id_type sourceId;
		TilePosition position;
	};
}
