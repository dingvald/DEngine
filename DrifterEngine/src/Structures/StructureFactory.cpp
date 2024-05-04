#include "pch.h"
#include "StructureFactory.h"
#include "Spatial/Helpers.h"
#include "JSON/JSONHelpers.h"

void drft::StructureFactory::createStructureBlueprintsFromJSON(const std::filesystem::path& directoryPath)
{
	for (const auto& filename : std::filesystem::directory_iterator(directoryPath))
	{
		auto optionalStuctureDocument = drft::json::extractDOM(filename.path(), "Structures");
		if (!optionalStuctureDocument.has_value())
		{
			std::cout << "Failure: " << directoryPath << " could not be parsed." << std::endl;
		}
		else
		{
			for (auto&& node : optionalStuctureDocument.value()["Structures"].GetObject())
			{
				StructureBlueprint blueprint{ node.name.GetString() };
				blueprint.createFromJSON(node.value);
				_blueprints.emplace(node.name.GetString(), blueprint);
			}
		}
	}
}

drft::StructureInstancePtr drft::StructureFactory::build(const std::string& name) const
{
	return std::move(_blueprints.at(name).build());
}
