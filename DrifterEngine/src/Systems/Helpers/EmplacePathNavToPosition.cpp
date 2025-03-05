#include "pch.h"
#include "EmplacePathNavToPosition.h"
#include <Spatial/WorldGrid.h>

#include <Components/PathNavComponent.h>
#include <Components/PositionComponent.h>
#include <Components/PhysicalBlockingComponent.h>

void drft::emplacePathNavToPosition(entt::handle entity, drft::TilePosition position)
{
	auto& grid = entity.registry()->ctx().get<spatial::WorldGrid>();

	if (auto positionComponent = entity.try_get<PositionComponent>())
	{
		auto path = grid.getPath(positionComponent->tile, position, [handle = entity](const std::vector<entt::entity>& entities) -> int
			{
				for (auto entity : entities)
				{
					if (handle.registry()->all_of<PhysicalBlockingComponent>(entity))
					{
						return 1000;
					}
				}
				return 0;
			});
		entity.emplace_or_replace<PathNavComponent>(std::move(path));
	}
}

void drft::emplacePathNavToPosition(entt::handle entity, drft::TilePosition position, std::function<void(entt::handle, drft::TilePosition)> onArrivalAction)
{
}
