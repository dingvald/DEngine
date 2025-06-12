#include "pch.h"
#include "EntityDecorator.h"
#include <Random/Random.h>
#include <ProcGen/GenerationContext.h>
#include <Spatial/Helpers.h>

void EntityDecorator::decorate(SlotPositionList& slotPositions, TaggedPositions& taggedPositions, const GenerationContext& context) const
{
	drft::rng::Random random = { generateUniqueSeed<EntityDecorator>(context.seed) };
	
	for (auto&& position : getMyPositions(taggedPositions))
	{
		if (!meetsCondition(random)) continue;
		
		generateCluster(position, slotPositions, random);
	}
}

void EntityDecorator::createFromJsonImpl(const rapidjson::Value& json)
{
	if (json.HasMember("radius"))
	{
		_radius.setMin(json["radius"].GetArray()[0].GetInt());
		_radius.setMax(json["radius"].GetArray()[1].GetInt());
	}
	if (json.HasMember("slots"))
	{
		for (auto&& slotObj : json["slots"].GetObject())
		{
			entt::id_type slotId = entt::hashed_string{ slotObj.name.GetString() };
			int number = slotObj.value["rolls"].GetInt();
			float chance = slotObj.value["chance"].GetFloat();

			_slots.emplace_back(slotId, number, chance);
		}
	}
}

void EntityDecorator::generateCluster(sf::Vector3i origin, SlotPositionList& slotPositions, drft::rng::Random& random) const
{
	auto radius = random.intInRange(_radius);
	for (auto&& [slot, num, chance] : _slots)
	{
		for (int i = 0; i < num; i++)
		{
			if (random.realInRange(0.f, 1.f) > chance) continue;

			auto point = random.positionInCircle(drft::spatial::toXY(origin), static_cast<int>(radius));
			slotPositions.emplace_back(slot, sf::Vector3i{ point.x, point.y, origin.z });
		}
	}
}
