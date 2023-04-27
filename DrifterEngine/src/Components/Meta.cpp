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
		.data<&Position::position>("position"_hs)
		.data<&Position::depth>("depth"_hs);

	snapshot::reflectComponent<Render, RENDER_STRING>()
		.prop("serialize"_hs)
		.data<&Render::sprite>("sprite"_hs)
		.data<&Render::color>("color"_hs);

	snapshot::reflectComponent<Physical, PHYSICAL_STRING>()
		.prop("serialize"_hs)
		.data<&Physical::weight>("weight"_hs)
		.data<&Physical::materials>("materials"_hs);

	snapshot::reflectComponent<Actor, ACTOR_STRING>()
		.prop("serialize"_hs)
		.data<&Actor::ap>("ap"_hs)
		.data<&Actor::moveSpeed>("moveSpeed"_hs)
		.data<&Actor::actSpeed>("actSpeed"_hs);

	snapshot::reflectComponent<Health, HEALTH_STRING>()
		.prop("serialize"_hs)
		.data<&Health::max>("max"_hs)
		.data<&Health::current>("current"_hs);

	snapshot::reflectComponent<Attacker, ATTACKER_STRING>()
		.prop("serialize"_hs)
		.data<&Attacker::baseDamage>("baseDamage"_hs);

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

	snapshot::reflectComponent<Container, CONTAINER_STRING>()
		.prop("serialize"_hs);
}
