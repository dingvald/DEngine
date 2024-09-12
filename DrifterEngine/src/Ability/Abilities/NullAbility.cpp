#include "pch.h"
#include "NullAbility.h"
#include "Events/SendFloatingMessageEvent.h"
#include "Components/PositionComponent.h"
#include <Spatial/Helpers.h>
#include <Spatial/Conversions.h>

drft::AbilityTargetingType drft::NullAbility::getTargetingType() const
{
	return AbilityTargetingType::Auto;
}

bool drft::NullAbility::isValid(entt::const_handle actor) const
{
	return false;
}

void drft::NullAbility::perform(entt::handle actor, std::optional<TilePosition> targetPosition) const
{
	const sf::Vector2f messagePosition = spatial::toXY(spatial::toFloatSpace(actor.get<PositionComponent>().tile));
	auto& dispatcher = actor.registry()->ctx().get<entt::dispatcher&>();
	dispatcher.trigger(events::SendFloatingMessageEvent {
		.message = "Cannot perform action.",
		.color = sf::Color::Red,
		.tracksEntity = actor.entity(),
		.position = messagePosition,
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

sf::IntRect drft::NullAbility::getTextureUV() const
{
	return { 0,0,16,16 };
}
