#include "pch.h"
#include "GetExperienceFromKilling.h"
#include "Components/LevelingComponent.h"

unsigned int drft::system::getExperienceFromKilling(entt::const_handle entity)
{
    unsigned int amount = 0;
    if (auto leveling = entity.try_get<LevelingComponent>())
    {
        amount = leveling->currentLevel * std::sqrt(leveling->neededXP);
    }
    return amount;
}
