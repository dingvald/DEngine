#include "pch.h"
#include "HasLineOfSight.h"
#include "Spatial/Helpers.h"
#include "Spatial/WorldGrid.h"
#include "Components/Components.h"

bool drft::system::hasLineOfSight(entt::const_handle entity, sf::Vector2i targetPosition)
{
	if (auto posComp = entity.try_get<component::Position>())
	{
		auto tilesInLOS = spatial::getIntPointsAlongLine(posComp->position, targetPosition);
		const auto& grid = entity.registry()->ctx().get<const spatial::WorldGrid&>();
		for (auto tile : tilesInLOS)
		{
			auto entities = grid.entitiesAt(tile,
				[&entity](entt::entity e) -> bool
				{
					const bool blocksLight = entity.registry()->any_of<component::LightBlocking>(e);
					return blocksLight;
				});

			if (!entities.empty()) return false;
		}
		return true;
	}
	return false;
}
