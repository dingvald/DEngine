#include "pch.h"
#include "StructureBlueprint.h"

drft::StructureBlueprint::StructureBlueprint(std::string name)
	: _name(name)
{}

void drft::StructureBlueprint::createFromJSON(const rapidjson::Value & json)
{
}

drft::StructureInstancePtr drft::StructureBlueprint::build() const
{
	// Where the magic happens
	return StructureInstancePtr();
}
