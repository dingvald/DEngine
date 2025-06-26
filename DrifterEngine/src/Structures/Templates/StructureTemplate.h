#pragma once
#include <Structures/Templates/StructureTemplateLayer.h>
#include <JSON/ICreateFromJson.h>

class StructureTemplate : public ICreateFromJson
{
public:
	void createFromJson(const rapidjson::Value& json) override;



private:
	std::unordered_map<int, StructureTemplateLayer> _layers;
};