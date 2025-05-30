#pragma once
#include <EnTT/entt.hpp>
#include <Spatial/TilePosition.h>

namespace drft::events
{
	struct PlayerTransferRequestEvent
	{
		std::optional<entt::id_type> sourceId;
		TilePosition position;
	};
}
