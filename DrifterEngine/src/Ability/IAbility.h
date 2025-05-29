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

	enum class AbilityUseMode
	{
		Activated,
		Sustained
	};

	static inline const std::unordered_map<AbilityUseMode, std::string> AbilityUseModeToString =
	{
		{ AbilityUseMode::Activated, "activated" },
		{ AbilityUseMode::Sustained, "sustained"},
	};

	enum class AbilityResourceType
	{
		Health, 
		Stamina,
	};

	static inline const std::unordered_map<AbilityResourceType, std::string> AbilityResourceCostToString =
	{
		{ AbilityResourceType::Health, "Health" },
		{ AbilityResourceType::Stamina, "Stamina" },
	};

	class IAbility
	{
	public:
		using ResourceCosts = std::unordered_map<AbilityResourceType, float>;
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

		virtual int getTimeCost() const { return 100; };
		virtual ResourceCosts getResourceCosts(entt::const_handle actor) const { return {}; }
		virtual AbilityUseMode getUseMode() const { return AbilityUseMode::Activated; };
		virtual bool isSustained(entt::const_handle actor) const { return false; }

		virtual entt::id_type getAssociatedSkill() const = 0;

	protected:
		bool hasResources(entt::const_handle actor) const;
		void spendResources(entt::handle actor) const;
		
	private:
		std::string _name;
	};
}


