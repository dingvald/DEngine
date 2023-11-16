#include "pch.h"
#include "TargetSelectors.h"
#include "Systems/Gameplay/FactionSystem.h"

bool drft::system::targetSelector::isHostile(entt::const_handle actor, entt::const_handle other)
{
    if (FactionSystem::resolveRelationship(actor, other) == Relationship::Hostile) return true;
    return false;
}
