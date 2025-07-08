#pragma once
#include <JSON/ICreateFromJson.h>
#include <Structures/Instance/StructureInstanceLayer.h>

class StructureTemplateLayer : public ICreateFromJson
{
public:
	void createFromJson(const rapidjson::Value& json) override;

	StructureInstanceLayer generate() const;

private:
	sf::IntRect _area;
};