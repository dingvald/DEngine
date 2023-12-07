#include "pch.h"
#include "NullAbility.h"
#include "Events/SendFloatingMessageEvent.h"
#include "Components/Components.h"

drft::AbilityTargetingType drft::NullAbility::getTargetingType() const
{
	return AbilityTargetingType::Auto;
}

bool drft::NullAbility::isValid(entt::const_handle actor) const
{
	return false;
}

void drft::NullAbility::perform(entt::handle actor, std::optional<sf::Vector2i> targetPosition) const
{
	auto& dispatcher = actor.registry()->ctx().get<entt::dispatcher&>();
	dispatcher.trigger(events::SendFloatingMessageEvent{
		.message = "Cannot perform action.",
		.color = sf::Color::Red,
		.tracksEntity = actor.entity(),
		.position = actor.get<component::Position>().position,
		.velocity = {0,-0.2},
		.fades = true,
		.isScreenSpace = false,
		.ttl = 80
		});
}

sf::Color drft::NullAbility::getIconColor() const
{
	return sf::Color::Magenta;
}

int drft::NullAbility::getSpriteIndex() const
{
	return 0;
}
