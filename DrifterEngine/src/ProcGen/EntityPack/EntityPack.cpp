#include "pch.h"
#include "EntityPack.h"
#include <Random/Random.h>
#include <Utility/StandardLogger.h>

void EntityPack::createFromJson(const rapidjson::Value& json)
{
	if (json.HasMember("slots"))
	{
		for (auto&& slotObj : json["slots"].GetObject())
		{
			auto slotId = entt::hashed_string{ slotObj.name.GetString() };
			if (slotId == EMPTY_ENTITY_SLOT)
			{
				LOG_ERROR("Entity slot {} clashes with reserved slot", slotId.data());
				continue;
			}
			WeightedEntityList list;
			for (auto&& weightedEntity : slotObj.value.GetArray())
			{
				auto pair = weightedEntity.GetArray();
				entt::id_type entityId = entt::hashed_string{ pair[0].GetString() };
				int entityWeight = pair[1].GetInt();
				list.emplace(entityId, entityWeight);
			}
			_packs.emplace(slotId, std::move(list));
		}
	}
}

void EntityPack::add(const EntityPack& other)
{
	for (auto&& [slotId, entityList] : other._packs)
	{
		if (_packs.contains(slotId))
		{
			auto& existingList = _packs.at(slotId);
			existingList.merge(EntityPack::WeightedEntityList{ entityList });
		}
		else
		{
			_packs.emplace(slotId, entityList);
		}
	}
}

std::optional<entt::id_type> EntityPack::selectEntity(entt::id_type slotId, drft::rng::Random& random) const
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

bool EntityPack::has(entt::id_type slotId) const
{
	return _packs.contains(slotId);
}


