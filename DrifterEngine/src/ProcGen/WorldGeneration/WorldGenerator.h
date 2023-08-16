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
		float getRangeFromPerlin(const std::string& mapName, double perlinValue) const;
		std::unordered_set<const Biome*> determinePotentialBiomes(sf::Vector2i coordinate) const;
		const Biome* selectBiome(sf::Vector2i coordinate) const;

	private:
		using NoiseMap = spatial::Grid<double>;
		unsigned int _seed = 0;
		sf::Vector2i _dimensions;
		std::vector<Biome> _biomeTypes;
		spatial::Grid<const Biome*> _biomeMap;

		std::unordered_map<std::string, NoiseMap> _noiseMaps;
		std::unordered_map<std::string, Range> _ranges;
	};
}


