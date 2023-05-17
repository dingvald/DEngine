#pragma once
#include "Components.h"

namespace cereal
{
	using namespace component;

	template<class Archive>
	void serialize(Archive& archive, Info& info)
	{
		archive(info.prototype, info.name, info.extendedName, info.description);
	}

	template<class Archive>
	void serialize(Archive& archive, Position& pos)
	{
		archive(pos.position.x, pos.position.y);
	}

	template<class Archive>
	void serialize(Archive& archive, Render& render)
	{
		archive(render.sprite, render.layer, render.color.r, render.color.g, render.color.b);
	}

	template<class Archive>
	void serialize(Archive& archive, LightSource& source)
	{
		archive(source.radius, source.dropOff, source.color.r, source.color.g, source.color.b);
	}

	template<class Archive>
	void serialize(Archive& archive, LightBlocking& lightBlocking)
	{
		archive(lightBlocking.opacity);
	}

	template<class Archive>
	void serialize(Archive& archive, Physical& physical)
	{
		archive(physical.blocks, physical.weight, physical.materials);
	}

	template<class Archive>
	void serialize(Archive& archive, Actor& actor)
	{
		archive(actor.ap, actor.moveSpeed, actor.actSpeed);
	}

	template<class Archive>
	void serialize(Archive& archive, Health& health)
	{
		archive(health.max, health.current);
	}

	template<class Archive>
	void serialize(Archive& archive, Attacker& attacker)
	{
		archive(attacker.baseDamage);
	}

	template<class Archive>
	void serialize(Archive& archive, Sharp& sharp)
	{
		archive(sharp.sharpness);
	}

	template<class Archive>
	void serialize(Archive& archive, Body& body)
	{
		archive(body.parts);
	}

	template<class Archive>
	void serialize(Archive& archive, Player& player)
	{
		archive(player.sightRange);
	}

	template<class Archive>
	void serialize(Archive& archive, AI& ai)
	{
		archive(ai.sightRange, ai.goals);
	}

	template<class Archive>
	void serialize(Archive& archive, Faction& faction)
	{
		archive(faction.name);
	}

	template<class Archive>
	void serialize(Archive& archive, Item& item)
	{
		archive(item.id);
	}

	template<class Archive>
	void serialize(Archive& archive, Container& container)
	{
		archive(container.contents);
	}

	template<class Archive>
	void serialize(Archive& archive, Craftable& craftable)
	{
		archive(craftable.recipe);
	}

	template<class Archive>
	void serialize(Archive& archive, MyCraftableItems& craftableItems)
	{
		archive(craftableItems.craftables, craftableItems.partialCraftables);
	}

	template<class Archive>
	void serialize(Archive& archive, Usable& usable)
	{
		archive(usable.action, usable.params);
	}
}



