#pragma once

#include "Components.h"

constexpr std::string_view PROTOTYPE_STRING = "Prototype";
constexpr std::string_view INFO_STRING = "Info";
constexpr std::string_view POSITION_STRING = "Position";
constexpr std::string_view RENDER_STRING = "Render";
constexpr std::string_view PLAYER_SEEN_STRING = "PlayerHasSeen";
constexpr std::string_view LIGHTSOURCE_STRING = "LightSource";
constexpr std::string_view LIGHT_BLOCKING_STRING = "LightBlocking";
constexpr std::string_view PHYSICAL_STRING = "Physical";
constexpr std::string_view LIQUID_STRING = "Liquid";
constexpr std::string_view IN_LIQUID_STRING = "InLiquid";
constexpr std::string_view ACTOR_STRING = "Actor";
constexpr std::string_view HEALTH_STRING = "Health";
constexpr std::string_view HEALING_STRING = "Healing";
constexpr std::string_view STAMINA_STRING = "Stamina";
constexpr std::string_view ATTACKER_STRING = "Attacker";
constexpr std::string_view SHARP_STRING = "Sharp";
constexpr std::string_view FLYING_STRING = "Flying";
constexpr std::string_view SPRINTING_STRING = "Sprinting";
constexpr std::string_view BODY_STRING = "Body";
constexpr std::string_view PLAYER_STRING = "Player";
constexpr std::string_view AI_STRING = "AI";
constexpr std::string_view FACTION_STRING = "Faction";
constexpr std::string_view ITEM_STRING = "Item";
constexpr std::string_view WEARABLE_STRING = "Wearable";
constexpr std::string_view CONTAINER_STRING = "Container";
constexpr std::string_view CRAFTABLE_STRING = "Craftable";
constexpr std::string_view CONSUMABLE_STRING = "Consumable";
constexpr std::string_view USABLE_STRING = "Usable";
constexpr std::string_view HORROR_STRING = "Horror";
constexpr std::string_view OPENABLE_STRING = "Openable";
constexpr std::string_view TICKING_LIFETIME_STRING = "TickingLifetime";
constexpr std::string_view LEVELING_STRING = "Leveling";
constexpr std::string_view BASE_STATS_STRING = "BaseStats";
constexpr std::string_view HOTBAR_STRING = "Hotbar";
constexpr std::string_view ANIMATION_STRING = "Animation";

namespace component
{
	class Meta
	{
	public:
		static void initialize();
	};
}


