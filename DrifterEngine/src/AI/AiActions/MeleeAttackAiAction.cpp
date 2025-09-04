#include "pch.h"
#include "MeleeAttackAiAction.h"

#include <Components/PositionComponent.h>
#include <Components/Actions/MeleeAttackAction.h>

#include <Spatial/Helpers.h>

bool MeleeAttackAiAction::isValid(entt::const_handle actor, OptionalTarget target) const
{
    return target.has_value();
}

void MeleeAttackAiAction::perform(entt::handle actor, OptionalTarget target) const
{
    auto* actorPosition = actor.try_get<PositionComponent>();
    if (!actorPosition) return;
    if (!target.has_value()) return;

    sf::Vector2i direction = drft::spatial::toXY(target.value() - actorPosition->tile);
    actor.emplace_or_replace<MeleeAttackAction>(direction);
}

bool MeleeAttackAiAction::isInRange(entt::const_handle actor, OptionalTarget target) const
{
    auto* position = actor.try_get<PositionComponent>();
    if (!position) return false;
    if (!target.has_value()) return false;

    const float distance = drft::spatial::distance3d(position->tile, target.value());
    return distance < ( sqrtf(2.0f) + 0.05f );
}
