#include "pch.h"
#include "EntityPackRegistry.h"
#include <JSON/JSONHelpers.h>
#include <Utility/StandardLogger.h>

using namespace drft;

static const EntityPack EmptyEntityPack = {};

void EntityPackRegistry::loadEntityPacks(const std::filesystem::path& entityPackDirectory)
{
	for (const auto& entry : std::filesystem::recursive_directory_iterator(entityPackDirectory))
	{
		if (entry.is_directory()) continue;

		json::JsonFileWrapper json{ entry.path(), "entity_pack" };
		if (!json.load())
		{
			//LOG_WARNING("{} could not be loaded", entry);
			continue;
		}
		for (auto&& packObj : json.getRoot().GetObject())
		{
			entt::id_type name = entt::hashed_string{ packObj.name.GetString() };
			EntityPack entityPack;
			entityPack.createFromJson(packObj.value);
			_entityPacks.emplace(std::move(name), std::move(entityPack));
		}
	}
}

const EntityPack& EntityPackRegistry::get(entt::id_type id) const
{
	if (!_entityPacks.contains(id))
	{
		LOG_WARNING("Could not find entity pack with id {} - Returning empty pack", id);
		return EmptyEntityPack;
	}
	return _entityPacks.at(id);
}
