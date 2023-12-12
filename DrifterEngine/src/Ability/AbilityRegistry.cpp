#include "pch.h"
#include "AbilityRegistry.h"
#include "Abilities/NullAbility.h"
#include "Abilities/ToggleSprintAbility.h"
#include "Abilities/ThrowAbility.h"

drft::AbilityRegistry::AbilityMap drft::AbilityRegistry::_abilities = {};

void drft::AbilityRegistry::bind()
{
    _abilities.emplace(AbilityType::NullAbility, std::make_unique<NullAbility>());
    _abilities.emplace(AbilityType::Sprint, std::make_unique<ToggleSprintAbility>());
    _abilities.emplace(AbilityType::Throw, std::make_unique<ThrowAbility>());
}

const drft::IAbility& drft::AbilityRegistry::get(AbilityType ability)
{
    if (!_abilities.contains(ability)) throw std::exception("Ability does not exist.");
    return *_abilities.at(ability);
}
