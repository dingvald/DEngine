#pragma once
#include <JSON/ICreateFromJson.h>

class StructureTemplateLayer : public ICreateFromJson
{
public:
	void createFromJson(const rapidjson::Value& json) override;

private:
	sf::IntRect _area;
};