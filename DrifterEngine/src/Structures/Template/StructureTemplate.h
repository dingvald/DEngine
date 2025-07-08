#pragma once
#include <Structures/Template/StructureTemplateLayer.h>
#include <JSON/ICreateFromJson.h>
#include <Spatial/AABB.h>

class StructureTemplate : public ICreateFromJson
{
public:
	void createFromJson(const rapidjson::Value& json) override;

private:
	std::unordered_map<int, StructureTemplateLayer> _layers;
};