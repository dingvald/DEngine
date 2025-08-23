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

    if (auto* actorPathNav = actor.try_get<PathNavComponent>())
    {
        // Path is already taken care of
        if (actorPathNav->path.back() == targetPosition->tile) return;
    }
    auto& grid = actor.registry()->ctx().get<drft::spatial::WorldGrid>();
    auto path = grid.getPath(actorPosition->tile, targetPosition->tile, drft::spatial::PhysicalBlockingHeuristic{*actor.registry()});
    actor.emplace_or_replace<PathNavComponent>(std::move(path));
}

float MoveToAiAction::range(entt::const_handle actor) const
{
    // should always be in range to move... right?
    return 0.0f;
}
