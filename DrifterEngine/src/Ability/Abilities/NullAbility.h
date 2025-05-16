#pragma once
#include "Ability/IAbility.h"

namespace drft
{
	class NullAbility : public IAbility
	{
		// Inherited via IAbility
		virtual AbilityTargetingType getTargetingType() const override;
		virtual bool isValid(entt::const_handle actor) const override;
		virtual void perform(entt::handle actor, std::optional<TilePosition> targetPosition) const override;
		

		// Inherited via IAbility
		AbilityIconData getIconData() const override;
	};
}


