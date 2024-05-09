#include "pch.h"
#include "StructureFactory.h"
#include "Spatial/Helpers.h"
#include "JSON/JSONHelpers.h"

#include "StructureInstance.h"

#include "StructureComponentFactory.h"
#include "Components/SquareComponent.h"

void drft::StructureFactory::bindStructureComponents() const
{
	StructureComponentFactory::registerComponent<SquareComponent>("Square");
}

void drft::StructureFactory::createStructureBlueprintsFromJSON(const std::filesystem::path& directoryPath)
{
	bindStructureComponents();

	for (const auto& filename : std::filesystem::directory_iterator(directoryPath))
	{
		auto optionalStuctureDocument = drft::json::extractDOM(filename.path(), "Structure");
		if (!optionalStuctureDocument.has_value())
		{
			std::cout << "Failure: " << directoryPath << " could not be parsed." << std::endl;
		}
		else
		{
			for (auto&& node : optionalStuctureDocument.value()["Structure"].GetObject())
			{
				StructureBlueprint blueprint{ node.name.GetString() };
				blueprint.createFromJSON(node.value);
				_blueprints.emplace(node.name.GetString(), std::move(blueprint));
			}
		}
	}
}

drft::StructureInstancePtr drft::StructureFactory::build(const std::string& name) const
{
	if (!_blueprints.contains(name))
	{
		std::cout << "StructureFactory does not contain blueprint name " << name << std::endl;
		return nullptr;
	}
	return std::move(_blueprints.at(name).build());
}