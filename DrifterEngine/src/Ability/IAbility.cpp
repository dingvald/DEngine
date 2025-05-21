#include "pch.h"
#include "IAbility.h"


drft::IAbility::IAbility(const char* name)
	: _name(name)
{
}

const std::string& drft::IAbility::getName() const
{
	return _name;
}

int drft::IAbility::getCost() const
{
	return 100;
}

bool drft::IAbility::isToggledOn(entt::const_handle actor) const
{
	return false;
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
