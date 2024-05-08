#include "pch.h"
#include "StructureBlueprint.h"
#include "StructureComponent.h"

drft::StructureBlueprint::StructureBlueprint(std::string name)
	: _name(name)
{}

void drft::StructureBlueprint::createFromJSON(const rapidjson::Value & json)
{
	
}

drft::StructureInstancePtr drft::StructureBlueprint::build() const
{
	std::unordered_map<entt::entity, PositionList> layout;
	for (auto&& comp : _components)
	{
		comp->apply(layout);
	}
	return std::make_unique<StructureInstance>(std::move(layout));
}
