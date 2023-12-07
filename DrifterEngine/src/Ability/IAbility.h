#pragma once
#include "AbilityTargetingType.h"
#include "Utility/Math.h"

namespace drft
{
	class IAbility
	{
	public:
		virtual sf::Color getIconColor() const = 0;
		virtual int getSpriteIndex() const = 0;

		virtual AbilityTargetingType getTargetingType() const = 0;
		virtual bool isValid(entt::const_handle actor) const = 0;
		virtual void perform(entt::handle actor, std::optional<sf::Vector2i> targetPosition = std::nullopt) const = 0;
		virtual int getCost() const;
		virtual math::Range<int> getRange(entt::const_handle actor) const;
		virtual std::vector<sf::Vector2i> getTargetingShape(entt::const_handle actor) const;
	private:
		std::string _name;
	};
}


