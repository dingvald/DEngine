#include "pch.h"
#include "HasLineOfSight.h"
#include "Spatial/Helpers.h"
#include "Spatial/WorldGrid.h"
#include "Components/PositionComponent.h"
#include "Components/LightBlockingComponent.h"

bool drft::system::hasLineOfSight(entt::const_handle entity, sf::Vector2i targetPosition)
{
	if (auto posComp = entity.try_get<PositionComponent>())
	{
		auto tilesInLOS = spatial::getIntPointsAlongLine(posComp->position, targetPosition);
		const auto& grid = entity.registry()->ctx().get<const spatial::WorldGrid&>();
		for (auto&& tile : tilesInLOS)
		{
			auto entities = grid.entitiesAt(tile,
				[&entity](entt::entity e) -> bool
				{
					return entity.registry()->any_of<LightBlockingComponent>(e);
				});

			if (!entities.empty()) return false;
		}
		return true;
	}
	return false;
}
