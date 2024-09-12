#pragma once
#include <Spatial/TilePosition.h>

namespace drft::system
{
	bool hasLineOfSight(entt::const_handle entity, TilePosition targetPosition);
}
