#include "pch.h"
#include "EntityPack.h"
#include <Random/Random.h>


void EntityPack::createFromJson(const rapidjson::Value& json)
{
	if (json.HasMember("slots"))
	{
		for (auto&& slotObj : json["slots"].GetObject())
		{
			auto slotId = entt::hashed_string{ slotObj.name.GetString() };
			WeightedEntityList list;
			for (auto&& weightedEntity : slotObj.value.GetArray())
			{
				auto pair = weightedEntity.GetArray();
				std::string entityName = pair[0].GetString();
				int entityWeight = pair[1].GetInt();
				list.emplace(entityName, entityWeight);
			}
			_packs.emplace(slotId, std::move(list));
		}
	}
}

void EntityPack::add(EntityPack other)
{
	for (auto&& [slotId, entityList] : other._packs)
	{
		if (_packs.contains(slotId))
		{
			auto& existingList = _packs.at(slotId);
			existingList.merge(entityList);
		}
		else
		{
			_packs.emplace(slotId, entityList);
		}
	}
}

std::optional<std::string> EntityPack::selectEntity(entt::id_type slotId, drft::rng::Random& random)
{
	if (!_packs.contains(slotId))
	{
		return std::nullopt;
	}
	auto& pack = _packs.at(slotId);

	if (auto selection = random.weightedSelection(pack))
	{
		return *selection;
	}
    return std::nullopt;
}


