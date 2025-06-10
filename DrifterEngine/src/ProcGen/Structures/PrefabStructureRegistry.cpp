#include "pch.h"
#include "PrefabStructureRegistry.h"
#include <Utility/StandardLogger.h>
#include <JSON/JSONHelpers.h>

static const PrefabStructure EmptyStructure = {};

void PrefabStructureRegistry::loadStructures(const std::filesystem::path& structuresDirectory)
{
	for (const auto& entry : std::filesystem::recursive_directory_iterator(structuresDirectory))
	{
		if (entry.is_directory()) continue;

		drft::json::JsonFileWrapper json{ entry.path() };
		if (!json.load())
		{
			LOG_WARNING("{} could not be loaded", entry.path().string());
			continue;
		}

		auto& structureValue = json.getRoot();
		auto structureName = structureValue["name"].GetString();
		PrefabStructure newStructure;
		newStructure.createFromJson(structureValue);

		_structures.emplace(entt::hashed_string{ structureName }, std::move(newStructure));
	}
}

const PrefabStructure& PrefabStructureRegistry::get(entt::id_type id) const
{
	if (!_structures.contains(id))
	{
		LOG_WARNING("Can not find structure with id {}", id);
		return EmptyStructure;
	}
	return _structures.at(id);
}