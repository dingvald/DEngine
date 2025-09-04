#include "pch.h"
#include "MoveToAiAction.h"

#include <Components/PositionComponent.h>
#include <Components/PathNavComponent.h>

#include <Spatial/WorldGrid.h>
#include <Spatial/PathingHeuristics/PhysicalBlockingHeuristic.h>

bool MoveToAiAction::isValid(entt::const_handle actor, OptionalTarget target) const
{
    auto* actorPosition = actor.try_get<PositionComponent>();
    if (!actorPosition) return false;

    return actorPosition->tile != target;
}

void MoveToAiAction::perform(entt::handle actor, OptionalTarget target) const
{
    auto* actorPosition = actor.try_get<PositionComponent>();
    if (!actorPosition) return;
    if (!target) return;

    auto& grid = actor.registry()->ctx().get<drft::spatial::WorldGrid>();
    if (auto* actorPathNav = actor.try_get<PathNavComponent>())
    {
        drft::spatial::WorldGrid::GridPath path = { actorPathNav->path.begin() + actorPathNav->progress, actorPathNav->path.end() };
        const bool isPathValid = grid.checkPath(path, drft::spatial::PhysicalBlockingHeuristic{ *actor.registry() });

        // Path is already taken care of
        if (isPathValid && actorPathNav->path.back() == target) return;
    }
    
    auto path = grid.getPath(actorPosition->tile, target.value(), drft::spatial::PhysicalBlockingHeuristic{*actor.registry()});
    actor.emplace_or_replace<PathNavComponent>(std::move(path));
}

bool MoveToAiAction::isInRange(entt::const_handle, OptionalTarget target) const
{
    return target.has_value();
}
