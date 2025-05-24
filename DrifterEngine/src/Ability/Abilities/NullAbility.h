#pragma once
#include "Ability/IAbility.h"

namespace drft
{
	class NullAbility : public IAbility
	{
	public:
		using IAbility::IAbility;

		virtual AbilityTargetingType getTargetingType() const override;
		virtual bool isValid(entt::const_handle actor) const override;
		virtual void perform(entt::handle actor, std::optional<TilePosition> targetPosition) const override;
		
		AbilityIconData getIconData() const override;

		entt::id_type getAssociatedSkill() const override;

		// Inherited via IAbility
		std::string getContextualDescription(entt::const_handle actor) const override;
	};
}


