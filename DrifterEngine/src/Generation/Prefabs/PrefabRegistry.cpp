#include "pch.h"
#include "PrefabRegistry.h"
#include <Utility/StandardLogger.h>
#include <JSON/JSONHelpers.h>

static const Prefab EmptyPrefab = {};

void PrefabRegistry::loadPrefabs(const std::filesystem::path& prefabsDirectory)
{
	for (const auto& entry : std::filesystem::recursive_directory_iterator(prefabsDirectory))
	{
		if (entry.is_directory()) continue;

		drft::json::JsonFileWrapper json{ entry.path() };
		if (!json.load())
		{
			LOG_WARNING("{} could not be loaded", entry.path().string());
			continue;
		}

		auto& prefabValue = json.getRoot();
		auto prefabName = prefabValue["name"].GetString();
		Prefab newPrefab;
		newPrefab.createFromJson(prefabValue);

		_prefabs.emplace(entt::hashed_string{ prefabName }, std::move(newPrefab));
	}
}

const Prefab& PrefabRegistry::get(entt::id_type id) const
{
	if (!_prefabs.contains(id))
	{
		LOG_WARNING("Can not find prefab with id {}", id);
		return EmptyPrefab;
	}
	return _prefabs.at(id);
}