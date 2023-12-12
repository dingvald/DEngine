#include "pch.h"
#include "ThrowAbility.h"

sf::Color drft::ThrowAbility::getIconColor() const
{
	return sf::Color::Magenta;
}

int drft::ThrowAbility::getSpriteIndex() const
{
	return 2;
}

drft::AbilityTargetingType drft::ThrowAbility::getTargetingType() const
{
	return AbilityTargetingType::SelectSquare;
}

bool drft::ThrowAbility::isValid(entt::const_handle actor) const
{
	// Should check if you have something in your hand to throw
	return true;
}

void drft::ThrowAbility::perform(entt::handle actor, std::optional<sf::Vector2i> targetPosition) const
{
}

drft::math::Range<int> drft::ThrowAbility::getRange(entt::const_handle actor) const
{
	return {0, 9};
}

std::vector<sf::Vector2i> drft::ThrowAbility::getTargetingShape(entt::const_handle actor) const
{
	return { sf::Vector2i{0,0} };
}
