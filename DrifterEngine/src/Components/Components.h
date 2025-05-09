#pragma once
#include "Utility/Math.h"


namespace component::action
{
	struct HotbarPressed
	{
		int slot = 0;
	};

	struct ToggleSprint {};

	struct SelectDirection
	{
		std::function<bool(sf::Vector2i)> onDirectionSelect;
	};

	struct SelectTarget
	{
		drft::math::Range<int> range = { 0,1 };
		std::vector<sf::Vector2i> targetShape = {};
		std::function<bool(sf::Vector3i)> onTargetSelect;
	};

	struct ToggleDoor {};

	struct IncomingDamage
	{
		std::unordered_map<std::string, int> damageTypes;
		entt::entity source = entt::null;
		entt::entity itemUsed = entt::null;
	};

	struct TakeDamage
	{
		int amount = 0;
		entt::entity source = entt::null;
	};

	struct PickUp {};

	struct SpendPoints
	{
		int amount = 0;
	};

	struct Drop
	{
		std::vector<unsigned long> toDrop;
	};

	struct Equip
	{
		unsigned long toEquip = 0;
		std::string slotName = {};
		unsigned int uid = 0;
	};

	struct Unequip
	{
		unsigned int toUnequip = 0;
	};

	struct Craft
	{
		std::string itemName;
		std::unordered_map<std::string, int> recipe;
	};

	struct Die {};

	struct OpenEquipment {};

	struct OpenWorldMap {};

	struct OpenCrafting {};

	struct ConsumeEntity
	{
		entt::entity consumed;
	};

	struct ConsumeStamina
	{
		float amount = 0.f;
	};

	struct GainExperience
	{
		unsigned int amount = 0;
	};

	struct LevelUp 
	{
		std::unordered_map<std::string, int> statChanges;
	};
}