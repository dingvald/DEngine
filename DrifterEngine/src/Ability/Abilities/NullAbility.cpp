#include "pch.h"
#include "NullAbility.h"

drft::AbilityTargetingType drft::NullAbility::getTargetingType() const
{
	return AbilityTargetingType::Auto;
}

bool drft::NullAbility::isValid(entt::const_handle actor) const
{
	return true;
}

void drft::NullAbility::perform(entt::handle actor, std::optional<sf::Vector2i> targetPosition) const
{
	std::cout << "This is from the null ability!" << std::endl;
}
