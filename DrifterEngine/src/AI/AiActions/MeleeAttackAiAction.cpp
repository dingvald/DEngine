#include "pch.h"
#include "MeleeAttackAiAction.h"

#include <Components/PositionComponent.h>
#include <Components/Actions/MeleeAttackAction.h>

#include <Spatial/Helpers.h>
#include <Utility/EntityHelpers.h>

bool MeleeAttackAiAction::canPerform(entt::const_handle actor) const
{
    return true;
}

void MeleeAttackAiAction::perform(entt::handle actor, entt::const_handle target) const
{
    auto* actorPosition = actor.try_get<PositionComponent>();
    auto* targetPosition = target.try_get<PositionComponent>();
    if (!actorPosition || !targetPosition)
    {
        return;
    }
    sf::Vector2i direction = drft::spatial::toXY(targetPosition->tile - actorPosition->tile);
    actor.emplace_or_replace<MeleeAttackAction>(direction);
}

bool MeleeAttackAiAction::isInRange(entt::const_handle actor, entt::const_handle target) const
{
    const float distance = drft::util::getDistanceBetween(actor, target);
    return distance < ( sqrtf(2.0f) + 0.05f );
}
