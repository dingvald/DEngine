#pragma once
#include "Ability/IAbility.h"
namespace drft
{
	class ToggleSprintAbility : public IAbility
	{
	public:
		using IAbility::IAbility;

		virtual AbilityTargetingType getTargetingType() const override;
		virtual bool isValid(entt::const_handle actor) const override;
		virtual void perform(entt::handle actor, std::optional<TilePosition> targetPosition) const override;
		virtual int getTimeCost() const override;
		virtual IAbility::ResourceCosts getResourceCosts(entt::const_handle actor) const override;
		virtual AbilityUseMode getUseMode() const override { return AbilityUseMode::Sustained; }
		virtual bool isSustained(entt::const_handle actor) const override;

		AbilityIconData getIconData() const override;

		entt::id_type getAssociatedSkill() const override;

		std::string getContextualDescription(entt::const_handle actor) const override;

	private:
		float calculateSpeedMultiplier(entt::const_handle actor) const;
		float calculateStaminaCost(entt::const_handle actor) const;
	};
}


