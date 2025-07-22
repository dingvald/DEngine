#include "pch.h"
#include "StructureRegistry.h"
#include <Utility/StandardLogger.h>
#include <JSON/JSONHelpers.h>

static const StructureTemplate EmptyStructureTemplate = {};

void StructureRegistry::loadStructures(const std::filesystem::path& structuresDirectory)
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
		StructureTemplate newStructure;
		newStructure.createFromJson(structureValue);

		_structures.emplace(entt::hashed_string{ structureName }, std::move(newStructure));
	}
}

const StructureTemplate& StructureRegistry::get(entt::id_type id) const
{
	if (!_structures.contains(id))
	{
		LOG_WARNING("Can not find structure with id {}", id);
		return EmptyStructureTemplate;
	}
	return _structures.at(id);
}
