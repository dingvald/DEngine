#pragma once
#include "Ability/IAbility.h"

namespace drft
{
	class PowerAttackAbility : public IAbility
	{
	public:
		using IAbility::IAbility;

		AbilityTargetingType getTargetingType() const override;
		bool isValid(entt::const_handle actor) const override;
		void perform(entt::handle actor, std::optional<TilePosition> targetPosition) const override;

		int getTimeCost() const override;
		IAbility::ResourceCosts getResourceCosts(entt::const_handle actor) const override;
		math::Range<int> getRange(entt::const_handle actor) const override;
		std::vector<sf::Vector2i> getTargetingShape(entt::const_handle actor) const override;

		AbilityIconData getIconData() const override;

		entt::id_type getAssociatedSkill() const override;

		std::string getContextualDescription(entt::const_handle actor) const override;

	private:
		void onTweenReachedTarget(entt::handle actor, drft::TilePosition targetPosition) const;
		void onTweenReturnedToStart(entt::handle actor) const;
		float calculatePowerAttackForce(entt::const_handle actor, entt::const_handle item) const;
	};
}