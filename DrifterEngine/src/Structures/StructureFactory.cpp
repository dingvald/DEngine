#include "pch.h"
#include "StructureFactory.h"
#include "Spatial/Helpers.h"
#include "JSON/JSONHelpers.h"

#include "StructureInstance.h"

#include "StructureShapeFactory.h"
#include "Shapes/RectangleShape.h"


void drft::StructureFactory::bindStructureShapes() const
{
	StructureShapeFactory::registerShape<RectangleShape>("Rectangle");
}

void drft::StructureFactory::createStructureBlueprintsFromJSON(const std::filesystem::path& directoryPath)
{
	bindStructureShapes();

	for (const auto& filename : std::filesystem::directory_iterator(directoryPath))
	{
		auto optionalStuctureDocument = drft::json::extractDOM(filename.path(), "Structure");
		if (!optionalStuctureDocument.has_value())
		{
			std::cout << "Failure: " << filename << " could not be parsed." << std::endl;
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