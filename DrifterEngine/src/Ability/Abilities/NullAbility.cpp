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

bool drft::NullAbility::isValid(entt::const_handle) const
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

drft::AbilityIconData drft::NullAbility::getIconData() const
{
	return AbilityIconData{ 
		.textureId = entt::hashed_string{"icons"}, 
		.color = sf::Color::Magenta, 
	};
}

entt::id_type drft::NullAbility::getAssociatedSkill() const
{
	return SkillId::None;
}

std::string drft::NullAbility::getContextualDescription(entt::const_handle actor) const
{
	return "This ability does nothing.";
}
