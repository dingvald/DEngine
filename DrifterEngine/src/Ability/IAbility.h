#pragma once
#include "AbilityTargetingType.h"
#include "Utility/Math.h"

#include <Spatial/TilePosition.h>
#include <Skills/SkillIds.h>

namespace drft
{
	struct AbilityIconData
	{
		entt::id_type textureId;
		sf::Color color;
		sf::Vector2i uv = { 0, 0 };
		sf::Vector2i uvSize = { 16, 16 };
	};

	class IAbility
	{
	public:
		IAbility(const char* name);
		virtual AbilityIconData getIconData() const = 0;
		const std::string& getName() const;
		virtual std::string getContextualDescription(entt::const_handle actor) const = 0;

		virtual bool isValid(entt::const_handle actor) const = 0;
		virtual void perform(entt::handle actor, std::optional<TilePosition> targetPosition = std::nullopt) const = 0;

		virtual AbilityTargetingType getTargetingType() const = 0;
		virtual math::Range<int> getRange(entt::const_handle actor) const;
		virtual std::vector<sf::Vector2i> getTargetingShape(entt::const_handle actor) const;

		virtual int getCost() const;
		virtual bool isToggledOn(entt::const_handle actor) const;

		virtual entt::id_type getAssociatedSkill() const = 0;
		
	private:
		std::string _name;
	};
}


