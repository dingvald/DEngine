#include "pch.h"
#include "StructureBlueprint.h"
#include "StructureInstance.h"
#include "StructureBaseShape.h"
#include "StructureDecorationFactory.h"

drft::StructureBlueprint::StructureBlueprint(std::string name)
	: _name(name)
{}

void drft::StructureBlueprint::createFromJSON(const rapidjson::Value & json)
{
	if (json.HasMember("BaseShape"))
	{

	}
	if (json.HasMember("Decorations"))
	{
		if (!json["Decorations"].IsArray())
		{
			throw std::exception("Decorations must be an array.");
			return;
		}
		for (auto&& decoration : json["Decorations"].GetArray())
		{
			auto decorationObj = decoration.GetObject().MemberBegin();
			auto decorationName = decorationObj->name.GetString();
			auto decorationInstance = StructureDecorationFactory::build(decorationName);
			if (!decorationInstance) continue;

			decorationInstance->createFromJSON(decorationObj->value);
			_decorations.emplace_back(std::move(decorationInstance));
		}
	}
}

drft::StructureInstancePtr drft::StructureBlueprint::build() const
{
	_baseShape->generateLayout();
	
	for (auto&& decoration : _decorations)
	{
		decoration->apply(*_baseShape);
	} 
	return std::make_unique<StructureInstance>(_baseShape->getLayout());
}
