#include "pch.h"
#include "IsHostile.h"
#include "Systems/Gameplay/FactionSystem.h"

bool drft::goap::filter::isHostile(entt::const_handle e1, entt::const_handle e2)
{
    return (system::FactionSystem::resolveRelationship(e1, e2) == system::Relationship::Hostile);
}
