#include "pch.h"
#include "AbilityRegistry.h"
#include "Abilities/NullAbility.h"

drft::AbilityRegistry::AbilityMap drft::AbilityRegistry::_abilities = {};

void drft::AbilityRegistry::bind()
{
    _abilities.emplace(AbilityType::NullAbility, std::make_unique<NullAbility>());
}

const drft::IAbility& drft::AbilityRegistry::get(AbilityType ability)
{
    if (!_abilities.contains(ability)) throw std::exception("Ability does not exist.");
    return *_abilities.at(ability);
}
