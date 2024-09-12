#include "pch.h"
#include "HasLineOfSight.h"
#include "Spatial/Helpers.h"
#include "Spatial/WorldGrid.h"
#include "Components/PositionComponent.h"
#include "Components/LightBlockingComponent.h"

bool drft::system::hasLineOfSight(entt::const_handle entity, TilePosition targetPosition)
{
	if (auto posComp = entity.try_get<PositionComponent>())
	{
		const auto& grid = entity.registry()->ctx().get<const spatial::WorldGrid&>();
		auto filterForLightBlocking = [&entity](entt::entity e) -> bool
			{
				return entity.registry()->all_of<LightBlockingComponent>(e);
			};
		auto entities = grid.castRay(posComp->tile, targetPosition, filterForLightBlocking);
		return entities.empty();
	}
	return false;
}
