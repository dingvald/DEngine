#include "pch.h"
#include "GetExperienceFromKilling.h"
#include "Components/Components.h"

unsigned int drft::system::getExperienceFromKilling(entt::entity entity, entt::registry& registry)
{
    unsigned int amount = 0;
    if (auto leveling = registry.try_get<component::Leveling>(entity))
    {
        amount = leveling->currentLevel * std::sqrt(leveling->neededXP);
    }
    return amount;
}
