#pragma once
#include "Random/PerlinNoise.h"
#include "Biome.h"
#include "Spatial/Grid.h"

namespace drft::gen
{
	class WorldGenerator
	{
	public:
		WorldGenerator();
		void setSeed(unsigned int seed);
		void loadBiomes(const std::string& JSONfilename);
		void generateTerrain(sf::Vector2i dimensions);

		void finalize(sf::Vector2i coordinate, entt::registry& registry) const;

		BiomeIcon getBiomeIcon(sf::Vector2i coordinate) const;

	private:
		TemperatureRange convertTemperatureFromPerlin(double perlinTemperature) const;
		HumidityRange convertHumidityFromPerlin(double perlinHumidity) const;
		AltitudeRange convertAltitudeFromPerlin(double perlinAltitude) const;
		std::unordered_set<Biome*> biomesThatSatisfy(TemperatureRange temperature, HumidityRange humidity, AltitudeRange altitude);

	private:
		unsigned int _seed = 0;
		std::vector<Biome> _biomeTypes;
		spatial::Grid<Biome*> _biomeMap;
		using NoisePtr = std::unique_ptr<rng::PerlinNoise>;
		NoisePtr _temperatureNoise;
		NoisePtr _altitudeNoise;
		NoisePtr _moistureNoise;
	};
}


