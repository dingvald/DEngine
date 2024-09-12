#pragma once
#include "EnTT/entt.h"
#include "SFML/Graphics.hpp"
#include <Spatial/TilePosition.h>

namespace drft::events
{
	struct LeaveTileEvent
	{
		entt::entity entity;
		TilePosition tilePosition;
	};
}