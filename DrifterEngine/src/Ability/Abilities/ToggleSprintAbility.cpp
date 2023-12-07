#include "pch.h"
#include "ToggleSprintAbility.h"
#include "Components/Components.h"
#include "Events/SendFloatingMessageEvent.h"

sf::Color drft::ToggleSprintAbility::getIconColor() const
{
    return sf::Color(101,122,98);
}

int drft::ToggleSprintAbility::getSpriteIndex() const
{
    return 1;
}

drft::AbilityTargetingType drft::ToggleSprintAbility::getTargetingType() const
{
    return AbilityTargetingType::Auto;
}

bool drft::ToggleSprintAbility::isValid(entt::const_handle actor) const
{
    if (auto stamina = actor.try_get<component::Stamina>())
    {
        if (stamina->current > 0) return true;
    }
    return false;
}

void drft::ToggleSprintAbility::perform(entt::handle actor, std::optional<sf::Vector2i> targetPosition) const
{
	if (actor.all_of<component::Sprinting>())
	{
		actor.remove<component::Sprinting>();
	}
	else
	{
		actor.emplace<component::Sprinting>();
	}
}

int drft::ToggleSprintAbility::getCost() const
{
	return 0;
}

bool drft::ToggleSprintAbility::isToggledOn(entt::const_handle actor) const
{
	if (actor.all_of<component::Sprinting>())
	{
		return true;
	}
	return false;
}
