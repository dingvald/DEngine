#pragma once

namespace component
{
	struct Prototype 
	{};

	struct Info
	{
		std::string prototype = { "" };
		std::string name = { "" };
		std::string extendedName = { "" };
		std::string description = { "" };
	};

	struct Position
	{
		sf::Vector2f position = { 0,0 };
	};

	struct Render
	{
		unsigned int sprite = 4;
		unsigned int layer = 0;
		sf::Color color = sf::Color::Magenta;
	};

	struct Lit
	{
		sf::Color color = sf::Color::Black;
	};

	struct LightSource
	{
		float radius = 1;
		float dropOff = 0.1;
		sf::Color color = sf::Color::White;
	};

	struct GlobalLightSource
	{
		sf::Color color = sf::Color::White;
	};

	struct TempLightSource
	{
		float radius = 1;
		float dropOff = 0.1;
		sf::Color color = sf::Color::White;
	};

	struct LightBlocking
	{
		float opacity = 1.0;
	};

	struct Physical
	{
		bool blocks = false;
		float weight = 0.0;
		std::vector<std::string> materials = {};
	};

	struct Liquid
	{
		float volume = 0.0; // Litres
	};

	struct InLiquid
	{
		std::string liquidPrototype;
		float volume = 0.0;
	};

	struct Camera
	{
		sf::FloatRect viewport = { 0,0,0,0 };
		entt::entity target = { entt::null };
	};

	struct Actor
	{
		int ap = { 0 };
		float moveSpeed = { 1.0f };
		float actSpeed = { 1.0f };
	};

	struct Health
	{
		int max = 10;
		int current = 10;
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
		bool hasBeenApplied = false;
	};

	struct Body
	{
		std::unordered_map<std::string, unsigned long> parts;
	};

	struct Player
	{
		int sightRange = 10;
	};

	struct AI
	{
		int sightRange = 0;
		std::unordered_set<std::string> goals;
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

	struct Horror
	{
		bool standin = false;
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
		int damage = 0;
	};

	struct ToggleSprint {};

	struct Wait
	{};

	struct TakeDamage
	{
		int amount = 0;
	};

	struct PickUp
	{};

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
		std::string slotname = {};
	};

	struct Unequip
	{
		std::string slotname = {};
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

	struct OpenInventory {};

	struct OpenEquipment {};

	struct OpenWorldMap {};

	struct OpenCrafting {};

	struct ConsumeStamina
	{
		float amount = 0.f;
	};

	struct DisplayText
	{
		std::unordered_map<std::string, std::string> parameters;
	};
}