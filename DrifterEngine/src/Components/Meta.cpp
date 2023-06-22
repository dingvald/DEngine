#include "pch.h"
#include "Meta.h"
#include "Serialize.h"
#include "Snapshot/Reflection.h"
#include <Entt/core/hashed_string.hpp>

using namespace entt::literals;

void component::Meta::initialize()
{
	// Meta types initialized here. Use the current ones as examples to 'meta' new compenents

	snapshot::reflectComponent<Info, INFO_STRING>()
		.prop("serialize"_hs)
		.data<&Info::prototype>("prototype"_hs)
		.data<&Info::name>("name"_hs)
		.data<&Info::extendedName>("extendedName"_hs)
		.data<&Info::description>("description"_hs);

	snapshot::reflectComponent<Position, POSITION_STRING>()
		.prop("serialize"_hs)
		.data<&Position::position>("position"_hs);

	snapshot::reflectComponent<Render, RENDER_STRING>()
		.prop("serialize"_hs)
		.data<&Render::sprite>("sprite"_hs)
		.data<&Render::layer>("layer"_hs)
		.data<&Render::color>("color"_hs);

	snapshot::reflectComponent<BaseStats, BASE_STATS_STRING>()
		.prop("serialize"_hs)
		.data<&BaseStats::strength>("strength"_hs)
		.data<&BaseStats::agility>("agility"_hs)
		.data<&BaseStats::vitality>("vitality"_hs);

	snapshot::reflectComponent<PlayerHasSeen, PLAYER_SEEN_STRING>()
		.prop("serialize"_hs);

	snapshot::reflectComponent<LightSource, LIGHTSOURCE_STRING>()
		.prop("serialize"_hs)
		.data<&LightSource::radius>("radius"_hs)
		.data<&LightSource::dropOff>("dropOff"_hs)
		.data<&LightSource::color>("color"_hs);

	snapshot::reflectComponent<LightBlocking, LIGHT_BLOCKING_STRING>()
		.prop("serialize"_hs)
		.data<&LightBlocking::opacity>("opacity"_hs);

	snapshot::reflectComponent<Physical, PHYSICAL_STRING>()
		.prop("serialize"_hs)
		.data<&Physical::blocks>("blocks"_hs)
		.data<&Physical::weight>("weight"_hs)
		.data<&Physical::materials>("materials"_hs);

	snapshot::reflectComponent<Liquid, LIQUID_STRING>()
		.prop("serialize"_hs)
		.data<&Liquid::volume>("volume"_hs);

	snapshot::reflectComponent<InLiquid, IN_LIQUID_STRING>()
		.prop("serialize"_hs);

	snapshot::reflectComponent<Actor, ACTOR_STRING>()
		.prop("serialize"_hs)
		.data<&Actor::ap>("ap"_hs)
		.data<&Actor::moveSpeed>("moveSpeed"_hs)
		.data<&Actor::actSpeed>("actSpeed"_hs);

	snapshot::reflectComponent<Health, HEALTH_STRING>()
		.prop("serialize"_hs)
		.data<&Health::max>("max"_hs)
		.data<&Health::current>("current"_hs);

	snapshot::reflectComponent<Stamina, STAMINA_STRING>()
		.prop("serialize"_hs)
		.data<&Stamina::max>("max"_hs)
		.data<&Stamina::current>("current"_hs)
		.data<&Stamina::baseConsumption>("baseConsumption"_hs);

	snapshot::reflectComponent<Attacker, ATTACKER_STRING>()
		.prop("serialize"_hs)
		.data<&Attacker::baseDamage>("baseDamage"_hs);

	snapshot::reflectComponent<Sharp, SHARP_STRING>()
		.prop("serialize"_hs)
		.data<&Sharp::sharpness>("sharpness"_hs);

	snapshot::reflectComponent<Flying, FLYING_STRING>()
		.prop("serialize"_hs)
		.data<&Flying::standin>("standin"_hs);

	snapshot::reflectComponent<Sprinting, SPRINTING_STRING>()
		.prop("serialize"_hs);

	snapshot::reflectComponent<Body, BODY_STRING>()
		.prop("serialize"_hs)
		.data<&Body::parts>("parts"_hs);

	snapshot::reflectComponent<Player, PLAYER_STRING>()
		.prop("serialize"_hs)
		.data<&Player::sightRange>("sightRange"_hs);

	snapshot::reflectComponent<AI, AI_STRING>()
		.prop("serialize"_hs)
		.data<&AI::sightRange>("sightRange"_hs)
		.data<&AI::goals>("goals"_hs);

	snapshot::reflectComponent<Faction, FACTION_STRING>()
		.prop("serialize"_hs)
		.data<&Faction::name>("name"_hs);

	snapshot::reflectComponent<Item, ITEM_STRING>()
		.prop("serialize"_hs)
		.data<&Item::id>("id"_hs);

	snapshot::reflectComponent<Wearable, WEARABLE_STRING>()
		.prop("serialize"_hs)
		.data<&Wearable::slot>("slot"_hs)
		.data<&Wearable::protection>("protection"_hs);

	snapshot::reflectComponent<Container, CONTAINER_STRING>()
		.prop("serialize"_hs)
		.data<&Container::capacity>("capacity"_hs);

	snapshot::reflectComponent<Craftable, CRAFTABLE_STRING>()
		.prop("serialize"_hs)
		.data<&Craftable::recipe>("recipe"_hs);

	snapshot::reflectComponent<Usable, USABLE_STRING>()
		.prop("serialize"_hs)
		.data<&Usable::action>("action"_hs)
		.data<&Usable::params>("params"_hs)
		.data<&Usable::consumes>("consumes"_hs);

	snapshot::reflectComponent<Horror, HORROR_STRING>()
		.prop("serialize"_hs);

	snapshot::reflectComponent<Door, DOOR_STRING>()
		.prop("serialize"_hs)
		.data<&Door::keyName>("keyName"_hs);

	snapshot::reflectComponent<TickingLifetime, TICKING_LIFETIME_STRING>()
		.prop("serialize"_hs)
		.data<&TickingLifetime::ticksRemaining>("ticksRemaining"_hs);

	snapshot::reflectComponent<Leveling, LEVELING_STRING>()
		.prop("serialize"_hs)
		.data<&Leveling::currentLevel>("currentLevel"_hs);
}
