#pragma once
#include "Ability/IAbility.h"
namespace drft
{
	class ToggleSprintAbility : public IAbility
	{
		// Inherited via IAbility
		virtual sf::Color getIconColor() const override;
		virtual sf::IntRect getTextureUV() const override;
		virtual AbilityTargetingType getTargetingType() const override;
		virtual bool isValid(entt::const_handle actor) const override;
		virtual void perform(entt::handle actor, std::optional<sf::Vector2i> targetPosition) const override;
		virtual int getCost() const override;
		virtual bool isToggledOn(entt::const_handle actor) const override;
	};
}


