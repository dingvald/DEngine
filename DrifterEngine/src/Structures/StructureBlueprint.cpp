#include "pch.h"
#include "StructureBlueprint.h"
#include "StructureInstance.h"
#include "StructureShapeFactory.h"

drft::StructureBlueprint::StructureBlueprint(std::string name)
	: _name(name)
{}

void drft::StructureBlueprint::createFromJSON(const rapidjson::Value & json)
{
	if (json.HasMember("BaseShape"))
	{
		auto shapeObj = json["BaseShape"].GetObject().MemberBegin();
		auto shapeName = shapeObj->name.GetString();
		auto shapeInstance = StructureShapeFactory::build(shapeName);
		if (!shapeInstance)
		{
			std::cout << "Structure " << shapeName << " could not be created." << std::endl;
			return;
		}
		shapeInstance->createFromJSON(shapeObj->value);
		_baseShape = std::move(shapeInstance);
	}
	else
	{
		std::cout << "Structure " << _name << " has no BaseShape. Please add to the JSON definition." << std::endl;
		return;
	}
	if (json.HasMember("Decorators"))
	{
		if (!json["Decorators"].IsArray())
		{
			throw std::exception("Decorators must be an array.");
			return;
		}
		for (auto&& decorator : json["Decorators"].GetArray())
		{
			StructureDecorator newDecorator;
			newDecorator.createFromJSON(decorator);
			_decorators.emplace_back(std::move(newDecorator));
		}
	}
}

drft::StructureInstancePtr drft::StructureBlueprint::build() const
{
	auto shapeInstance = _baseShape->generate();
	for (auto&& decorator : _decorators)
	{
		decorator.apply(*shapeInstance);
	}
	return std::make_unique<StructureInstance>(std::move(shapeInstance));
}
