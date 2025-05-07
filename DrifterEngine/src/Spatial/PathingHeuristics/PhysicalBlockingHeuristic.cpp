#include "pch.h"
#include "PhysicalBlockingHeuristic.h"
#include <Components/PhysicalBlockingComponent.h>

drft::spatial::PhysicalBlockingHeuristic::PhysicalBlockingHeuristic(entt::registry& registry)
    : _registry(registry)
{
}

int drft::spatial::PhysicalBlockingHeuristic::evaluate(const EntityList& entities) const
{
	for (auto entity : entities)
	{
		if (_registry.all_of<PhysicalBlockingComponent>(entity))
		{
			return 10;
		}
	}
	return 0;
}
