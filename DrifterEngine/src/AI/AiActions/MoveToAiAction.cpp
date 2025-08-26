#include "pch.h"
#include "MoveToAiAction.h"

#include <Components/PositionComponent.h>
#include <Components/PathNavComponent.h>

#include <Spatial/WorldGrid.h>
#include <Spatial/PathingHeuristics/PhysicalBlockingHeuristic.h>

bool MoveToAiAction::canPerform(entt::const_handle actor) const
{
    return true;
}

void MoveToAiAction::perform(entt::handle actor, entt::const_handle target) const
{
    auto* actorPosition = actor.try_get<PositionComponent>();
    auto* targetPosition = target.try_get<PositionComponent>();
    if (!actorPosition || !targetPosition) return;

    auto& grid = actor.registry()->ctx().get<drft::spatial::WorldGrid>();
    if (auto* actorPathNav = actor.try_get<PathNavComponent>())
    {
        drft::spatial::WorldGrid::GridPath path = { actorPathNav->path.begin() + actorPathNav->progress, actorPathNav->path.end() };
        const bool isPathValid = grid.checkPath(path, drft::spatial::PhysicalBlockingHeuristic{ *actor.registry() });

        // Path is already taken care of
        if (isPathValid && actorPathNav->path.back() == targetPosition->tile) return;
    }
    
    auto path = grid.getPath(actorPosition->tile, targetPosition->tile, drft::spatial::PhysicalBlockingHeuristic{*actor.registry()});
    actor.emplace_or_replace<PathNavComponent>(std::move(path));
}

bool MoveToAiAction::isInRange(entt::const_handle, entt::const_handle) const
{
    // should always be in range to move... right?
    return true;
}
