#include "pch.h"
#include "EmplacePathNavToPosition.h"
#include <Spatial/WorldGrid.h>

#include <Components/PathNavComponent.h>
#include <Components/PositionComponent.h>
#include <Components/PhysicalBlockingComponent.h>

void drft::emplacePathNavToPosition(entt::handle entity, drft::TilePosition position)
{

}

void drft::emplacePathNavToPosition(entt::handle entity, drft::TilePosition position, std::function<void(entt::handle, drft::TilePosition)> onArrivalAction)
{
}
