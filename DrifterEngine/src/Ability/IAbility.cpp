#include "pch.h"
#include "IAbility.h"
#include <Ability/AbilityHelpers.h>


drft::IAbility::IAbility(const char* name)
	: _name(name)
{
}

const std::string& drft::IAbility::getName() const
{
	return _name;
}

drft::math::Range<int> drft::IAbility::getRange(entt::const_handle) const
{
	if (getTargetingType() == AbilityTargetingType::SelectSquare) throw std::exception("Need to override");
	return { 0,0 };
}

std::vector<sf::Vector2i> drft::IAbility::getTargetingShape(entt::const_handle actor) const
{
	if (getTargetingType() != AbilityTargetingType::Auto) throw std::exception("Need to override");
	return std::vector<sf::Vector2i>{};
}

bool drft::IAbility::hasResources(entt::const_handle actor) const
{
	return AbilityHelpers::hasResources(actor, *this);
}

void drft::IAbility::spendResources(entt::handle actor) const
{
	AbilityHelpers::spendResources(actor, *this);
}
