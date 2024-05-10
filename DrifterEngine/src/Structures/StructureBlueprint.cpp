#include "pch.h"
#include "StructureBlueprint.h"
#include "StructureInstance.h"
#include "StructureComponentFactory.h"

drft::StructureBlueprint::StructureBlueprint(std::string name)
	: _name(name)
{}

void drft::StructureBlueprint::createFromJSON(const rapidjson::Value & json)
{
	if (json.HasMember("Components"))
	{
		if (!json["Components"].IsArray())
		{
			throw std::exception("Components must be an array.");
			return;
		}
		for (auto&& component : json["Components"].GetArray())
		{
			auto componentObj = component.GetObject().MemberBegin();
			auto componentName = componentObj->name.GetString();
			auto componentInstance = StructureComponentFactory::build(componentName);
			if (!componentInstance) continue;

			componentInstance->createFromJSON(componentObj->value);
			_components.emplace_back(std::move(componentInstance));
		}
	}
}

drft::StructureInstancePtr drft::StructureBlueprint::build() const
{
	std::unordered_map<std::string, PositionList> layout;
	
	for (auto&& comp : _components)
	{
		comp->apply(layout);
	} 
	return std::make_unique<StructureInstance>(std::move(layout));
}
