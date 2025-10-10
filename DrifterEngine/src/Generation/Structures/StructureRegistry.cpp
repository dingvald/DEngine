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

		drft::json::JsonFileWrapper json{ entry.path(), "structure" };
		if (!json.load())
		{
			LOG_WARNING("{} could not be loaded", entry.path().string());
			continue;
		}

		for (auto&& structureObj : json.getRoot().GetObject())
		{
			entt::id_type name = entt::hashed_string{ structureObj.name.GetString() };
			StructureTemplate newStructure;
			newStructure.createFromJson(structureObj.value);
			_structures.emplace(std::move(name), std::move(newStructure));
		}
	}
}

void StructureRegistry::finalize(const GenerationFinalizationContext& context)
{
	for (auto&& [id, structure] : _structures)
	{
		structure.finalize(context);
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
