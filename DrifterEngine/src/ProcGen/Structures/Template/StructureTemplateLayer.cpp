#include "pch.h"
#include "StructureTemplateLayer.h"

void StructureTemplateLayer::createFromJson(const rapidjson::Value& json)
{
}

StructureInstanceLayer StructureTemplateLayer::generate() const
{
	return StructureInstanceLayer();
}
