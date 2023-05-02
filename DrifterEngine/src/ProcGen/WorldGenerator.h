#pragma once
#include "Biome.h"
#include "BiomeTypes.h"
#include "Random/PerlinNoise.h"

namespace drft::gen
{
	class WorldGenerator
	{
	public:
		WorldGenerator(unsigned int seed);
		bool loadBiomeBlueprints(std::string filename);
		void buildChunk(sf::Vector2i coordinate, entt::registry& registry) const;
		BiomePair getBiomeType(sf::Vector2i coordinate) const;

	private:
		BiomePair determineBiomeType(double temperature, double altitude, double moisture) const;

	private:
		unsigned int _seed = 0;

		using NoisePtr = std::unique_ptr<rng::PerlinNoise>;
		NoisePtr _temperatureNoise;
		NoisePtr _altitudeNoise;
		NoisePtr _moistureNoise;

		std::unordered_map<BiomeType, Biome> _biomes;
	};
}


