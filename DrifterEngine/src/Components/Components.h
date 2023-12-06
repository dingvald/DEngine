#pragma once
#include "Systems/HelperClasses/BodyParts.h"
#include "Systems/HelperClasses/AIStates.h"
#include "GOAP/WorldState.h"
#include "GOAP/Actions/AiActionTypes.h"
#include "GOAP/SensorTypes.h"

namespace drft::goap
{
	using Plan = std::deque<AiAction>;
}

namespace component
{
	struct Prototype 
	{};

	struct Info
	{
		std::string prototype = { "PROTOTYPE" };
		std::string name = { "" };
		std::string extendedName = { "" };
		std::string description = { "" };
	};

	struct Position
	{
		sf::Vector2i position = { 0,0 };
	};

	struct Render
	{
		unsigned int sprite = 4u;
		unsigned int layer = 0u;
		sf::Color color = sf::Color::Magenta;
	};

	struct BaseStats
	{
		int strength = 1;
		int agility = 1;
		int vitality = 1;
	};

	struct PlayerHasSeen
	{
		bool standin = false;
	};

	struct Lit
	{
		sf::Color color = sf::Color::Black;
	};

	struct LightSource
	{
		float radius = 1.f;
		float dropOff = 0.1f;
		sf::Color color = sf::Color::White;
	};

	struct GlobalLightSource
	{
		sf::Color color = sf::Color::White;
	};

	struct TempLightSource
	{
		float radius = 1.f;
		float dropOff = 0.1f;
		sf::Color color = sf::Color::White;
	};

	struct LightBlocking
	{
		float opacity = 1.0f;
	};

	struct Physical
	{
		bool blocks = false;
		float weight = 0.0f;
		std::vector<std::string> materials = {};
	};

	struct Liquid
	{
		float volume = 0.0f; // Litres
	};

	struct InLiquid
	{
		std::string liquidPrototype;
		float volume = 0.0f;
	};

	struct Camera
	{
		sf::FloatRect viewport;
		sf::Vector2f lag;
		entt::entity target = { entt::null };
	};

	struct Actor
	{
		int ap = 0;
		float moveSpeed = 1.0f;
		float actSpeed = 1.0f;
	};

	struct Health
	{
		float max = 10.f;
		float current = 10.f;
		float recovery = 0.f;
	};

	struct Stamina
	{
		float max = 10.f;
		float current = 10.f;
		float baseConsumption = 0.f;
	};

	struct Attacker
	{
		int baseDamage = 1;
	};

	struct Sharp
	{
		int sharpness = 1;
	};

	struct Flying
	{
		int standin = 0;
	};

	struct Sprinting
	{
		sf::Vector2i direction = { 0,0 };
	};

	struct Body
	{
		PartTree parts;
	};

	struct Wearable
	{
		std::vector<std::string> slots;
		int layer = 0;
		std::unordered_map<std::string, int> protections;
	};

	struct Player
	{
		int sightRange = 10;
	};

	struct AI
	{
		int sightRange = 0;
		std::unordered_set<std::string> goals;
		std::string currentGoal = {};
		drft::goap::Plan plan;
		drft::goap::WorldState blackboard;
		AIState state = AIState::Think;

		// Runtime only
		using SurroundingsMemory = std::unordered_map<drft::goap::SensorType, std::unordered_map<entt::entity, int>>;
		SurroundingsMemory surroundings;
		entt::entity target = entt::null;
	};

	struct Faction
	{
		std::string name;
	};

	struct Item
	{
		using ID = unsigned long;
		static const ID NONE = 0;
		ID id = 0;
	};

	struct Container
	{
		std::vector<Item::ID> contents;
		unsigned int capacity = 0;
	};

	struct Craftable
	{
		std::unordered_map<std::string, unsigned long> recipe;
	};

	struct MyCraftableItems
	{
		std::vector<entt::entity> craftables;
		std::vector<entt::entity> partialCraftables;
	};

	struct Usable
	{
		using Parameters = std::unordered_map<std::string, std::string>;
		std::string action;
		Parameters params;
		bool consumes = false;
	};

	struct Openable
	{
		bool isOpen = false;
		std::string keyName = "";
	};

	struct TickingLifetime
	{
		int ticksRemaining = 0;
	};

	struct Leveling
	{
		unsigned int currentLevel = 0;
		unsigned int currentXP = 0;
		unsigned int neededXP = 100;
	};

	struct Hotbar
	{
		std::array<int, 10> abilities;
	};
}

namespace component::action
{
	struct Move
	{
		sf::Vector2i direction = { 0,0 };
	};

	struct LaunchAttack
	{
		sf::Vector2i direction = { 0,0 };
		std::unordered_map<std::string, int> damageTypes;
	};

	struct HotbarPressed
	{
		int slot = 0;
	};

	struct ToggleSprint {};

	struct Wait {};

	struct TryInteract {};

	struct DoInteract 
	{
		entt::entity actor;
		entt::entity subject;
	};

	struct SelectDirection
	{
		std::function<bool(sf::Vector2i)> onDirectionSelect;
	};

	struct ToggleDoor {};

	struct IncomingDamage
	{
		std::unordered_map<std::string, int> damageTypes;
		entt::entity source = entt::null;
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
		std::vector<Item::ID> toDrop;
	};

	struct Equip
	{
		Item::ID toEquip = 0;
		std::string partName = {};
		EquipmentLayer layer = EquipmentLayer::Base;
	};

	struct Unequip
	{
		Item::ID toUnequip = 0;
	};

	struct Craft
	{
		std::string itemName;
		std::unordered_map<std::string, int> recipe;
	};

	struct Use
	{
		entt::entity entity;
		Item::ID item;
	};

	struct Die {};

	struct OpenEquipment {};

	struct OpenWorldMap {};

	struct OpenCrafting {};

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

	struct DisplayText
	{
		std::unordered_map<std::string, std::string> parameters;
	};
}