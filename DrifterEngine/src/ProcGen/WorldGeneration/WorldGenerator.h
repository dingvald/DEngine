#pragma once
#include "Random/PerlinNoise.h"
#include "ProcGen/Biome.h"
#include "Spatial/Grid.h"

namespace drft::gen
{
	class WorldGenerator
	{
	public:
		WorldGenerator();
		void init(sf::Vector2i dimensions, unsigned int seed);
		void loadBiomes(const std::string& JSONfilename);
		void generateTerrain();

		void finalize(sf::Vector2i coordinate, entt::registry& registry) const;

		BiomeIcon getBiomeIcon(sf::Vector2i coordinate) const;

	private:
		float getTemperatureFromPerlin(double perlinTemperature) const;
		float getHumidityFromPerlin(double perlinHumidity) const;
		float getAltitudeFromPerlin(double perlinAltitude) const;
		std::unordered_set<const Biome*> biomesThatSatisfy(float temperature, float humidity, float altitude) const;
		const Biome* selectBiome(sf::Vector2i coordinate) const;

	private:
		unsigned int _seed = 0;
		sf::Vector2i _dimensions;
		std::vector<Biome> _biomeTypes;
		spatial::Grid<const Biome*> _biomeMap;

		spatial::Grid<double> _altitudeMap;
		spatial::Grid<double> _temperatureMap;
		spatial::Grid<double> _humidityMap;

		Range _temperatureExtremes;
		Range _humidityExtremes;
		Range _altitudeExtremes;
	};
}


