#include "pch.h"
#include "AbilityRegistry.h"
#include "Abilities/NullAbility.h"
#include "Abilities/ToggleSprintAbility.h"
#include "Abilities/ThrowAbility.h"
#include <Ability/AbilityIds.h>

#include <Utility/StandardLogger.h>

drft::AbilityRegistry::AbilityMap drft::AbilityRegistry::_abilities = {};


void drft::AbilityRegistry::bind()
{
    _abilities.emplace(AbilityIds::NullAbility, std::make_unique<NullAbility>());
    _abilities.emplace(AbilityIds::SprintAbility, std::make_unique<ToggleSprintAbility>());
    _abilities.emplace(AbilityIds::ThrowAbility, std::make_unique<ThrowAbility>());
}

const drft::IAbility& drft::AbilityRegistry::get(entt::id_type abilityId)
{
    if (!_abilities.contains(abilityId))
    {
        LOG_WARNING("Trying to get unknown ability from ability registry");
        return *_abilities.at(AbilityIds::NullAbility);
    }
    return *_abilities.at(abilityId);
}
