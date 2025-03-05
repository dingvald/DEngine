#pragma once
#include <Spatial/TilePosition.h>

namespace drft
{
	void emplacePathNavToPosition(entt::handle entity, drft::TilePosition position);
	void emplacePathNavToPosition(entt::handle entity, drft::TilePosition position, std::function<void(entt::handle, drft::TilePosition)> onArrivalAction);
}