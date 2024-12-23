#include "pch.h"
#include "PathNavSystem.h"

#include <Components/CurrentActorComponent.h>
#include <Components/Actions/MoveAction.h>
#include <Components/PathNavComponent.h>
#include <Components/PositionComponent.h>
#include <Components/Tags.h>

#include <Spatial/Helpers.h>


void drft::system::PathNavSystem::update()
{
	auto view = _registry.view<PathNavComponent, PositionComponent, CurrentActorComponent>();
	for (auto [entity, nav, position, current] : view.each())
	{
		if (nav.progress >= nav.path.size())
		{
			_registry.remove<PathNavComponent>(entity);
			continue;
		}

		TilePosition nextPosition = nav.path.at(nav.progress);
		TilePosition delta = nextPosition - position.tile;

		_registry.emplace_or_replace<MoveAction>(entity, spatial::toXY(delta));
		nav.progress++;
	}
}
