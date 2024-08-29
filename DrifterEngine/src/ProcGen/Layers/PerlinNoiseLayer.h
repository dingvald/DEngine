#pragma once
#include <ProcGen/LayeredProcGen/LayeredProcGen.h>
#include <JSON/ICreateFromJson.h>
#include <Random/PerlinNoise.h>

class PerlinNoiseLayer : public OnDemandLayer, ICreateFromJson
{
public:
	PerlinNoiseLayer(sf::Vector2i dimensions, unsigned int seed);
	double getValueAt(sf::Vector2i tilePosition) override;
	void createFromJson(const rapidjson::Value& json) override;

private:
	double _getValueAt(sf::Vector2i tilePosition) const;

private:
	double _resolution = 1.0;
	sf::Vector2i _dimensions = { 1024, 1024 };
	int _octaves = 8;
	float _lacunarity = 2.0f;
	float _gain = 0.5f;
	drft::rng::PerlinNoise _noise;
	unsigned int _seed;
};