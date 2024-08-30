#pragma once
#include <ProcGen/LayeredProcGen/LayeredProcGen.h>
#include <JSON/ICreateFromJson.h>


class RandomLayer : public OnDemandLayer, public ICreateFromJson
{
public:
	double getValueAt(sf::Vector2i tilePosition) override;

	// Inherited via ICreateFromJson
	void createFromJson(const rapidjson::Value& json) override;

private:
	unsigned int _seed;
};