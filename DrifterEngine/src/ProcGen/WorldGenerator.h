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
		void init(sf::Vector2i dimensions, unsigned int seed);
		void loadBiomes(const std::string& JSONfilename);
		void generateTerrain();

		void finalize(sf::Vector2i coordinate, entt::registry& registry) const;

		BiomeIcon getBiomeIcon(sf::Vector2i coordinate) const;

		double getAltitude(sf::Vector2i tileCoordinate) const;
		double getHumidity(sf::Vector2i tileCoordinate) const;
		double getTemperature(sf::Vector2i tileCoordinate) const;

	private:
		sf::Vector2<double> normalizeCoordinates(sf::Vector2i tileCoordinate) const;
		TemperatureRange getTemperatureFromPerlin(double perlinTemperature) const;
		HumidityRange getHumidityFromPerlin(double perlinHumidity) const;
		AltitudeRange getAltitudeFromPerlin(double perlinAltitude) const;
		std::unordered_set<Biome*> biomesThatSatisfy(TemperatureRange temperature, HumidityRange humidity, AltitudeRange altitude);

	private:
		unsigned int _seed = 0;
		sf::Vector2i _dimensions;
		std::vector<Biome> _biomeTypes;
		spatial::Grid<Biome*> _biomeMap;
		using NoisePtr = std::unique_ptr<rng::PerlinNoise>;
		NoisePtr _temperatureNoise;
		NoisePtr _altitudeNoise;
		NoisePtr _moistureNoise;
	};
}


