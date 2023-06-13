#pragma once
#include "Biome.h"
#include "BiomeTypes.h"
#include "Random/PerlinNoise.h"

namespace drft::spatial
{
	template<typename T>
	class Grid;
}

namespace drft::gen
{
	enum class GenerationLayer
	{
		Environmentals,
		Actors,
		Structures,
		Props
	};

	class WorldGenerator
	{
	public:
		void setSeed(unsigned int seed);
		bool loadBiomeBlueprints(std::string filename);
		void buildChunk(sf::Vector2i coordinate, entt::registry& registry) const;
		BiomeType getBiomeType(sf::Vector2i coordinate) const;

	private:
		BiomeType determineBiomeType(double temperature, double altitude, double moisture) const;
		sf::Vector2<double> convertIntergerCoordinates(sf::Vector2i coord) const;
		std::vector<sf::Vector2i> determineOpenFaces(sf::Vector2i coord) const;
		void determineAvailableSpaces(std::vector<sf::Vector2i> openFaces, spatial::Grid<int>& spaces, unsigned int seed) const;

	private:
		unsigned int _seed = 0;

		using NoisePtr = std::unique_ptr<rng::PerlinNoise>;
		NoisePtr _temperatureNoise;
		NoisePtr _altitudeNoise;
		NoisePtr _moistureNoise;

		std::unordered_map<BiomeType, Biome> _biomes;
		mutable std::map<std::pair<int, int>, BiomeType> _cachedBiomeTypes;
	};
}


