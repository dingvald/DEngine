#include "pch.h"
#include "AbilityRegistry.h"
#include "Abilities/NullAbility.h"
#include "Abilities/ToggleSprintAbility.h"
#include "Abilities/ThrowAbility.h"
#include "Abilities/PowerAttackAbility.h"
#include <Ability/AbilityIds.h>

#include <Utility/StandardLogger.h>

drft::AbilityRegistry::AbilityMap drft::AbilityRegistry::_abilities = {};


void drft::AbilityRegistry::bind()
{
    AbilityRegistry::registerAbility<NullAbility>(AbilityIds::NullAbility);
    AbilityRegistry::registerAbility<ToggleSprintAbility>(AbilityIds::SprintAbility);
    AbilityRegistry::registerAbility<ThrowAbility>(AbilityIds::ThrowAbility);
    AbilityRegistry::registerAbility<PowerAttackAbility>(AbilityIds::PowerAttackAbility);
}

const drft::IAbility& drft::AbilityRegistry::get(entt::id_type abilityId)
{
    if (!_abilities.contains(abilityId))
    {
        LOG_WARNING("Trying to get unknown ability from ability registry");
        return *_abilities.at(entt::hashed_string{ AbilityIds::NullAbility });
    }
    return *_abilities.at(abilityId);
}
