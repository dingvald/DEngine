#pragma once
#include "AbilityTargetingType.h"
#include "Utility/Math.h"

#include <Spatial/TilePosition.h>

namespace drft
{
	struct AbilityIconData
	{
		entt::id_type textureId;
		sf::Color color;
		sf::Vector2i uv;
		sf::Vector2i uvSize;
	};

	class IAbility
	{
	public:
		virtual AbilityIconData getIconData() const = 0;

		virtual AbilityTargetingType getTargetingType() const = 0;
		virtual bool isValid(entt::const_handle actor) const = 0;
		virtual void perform(entt::handle actor, std::optional<TilePosition> targetPosition = std::nullopt) const = 0;

		virtual int getCost() const;
		virtual bool isToggledOn(entt::const_handle actor) const;
		virtual math::Range<int> getRange(entt::const_handle actor) const;
		virtual std::vector<sf::Vector2i> getTargetingShape(entt::const_handle actor) const;
	private:
		std::string _name;
	};
}


