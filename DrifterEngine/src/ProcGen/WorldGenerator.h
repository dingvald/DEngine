#pragma once
#include "Biome.h"
#include "BiomeTypes.h"
#include "PlacementAlgorithms/GridEnums.h"
#include "Random/PerlinNoise.h"
#include "Factory/MachineFactory.h"

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
		WorldGenerator();
		void setSeed(unsigned int seed);
		bool loadBiomeBlueprints(std::string filename);
		void buildChunk(sf::Vector2i coordinate, entt::registry& registry) const;
		BiomeType getBiomeType(sf::Vector2i coordinate) const;

	private:
		void registerMachines();
		BiomeType determineBiomeType(double temperature, double altitude, double moisture) const;
		sf::Vector2<double> convertIntergerCoordinatesToDouble(sf::Vector2i coord) const;
		std::vector<sf::Vector2i> determineOpenFaces(sf::Vector2i coord) const;
		void addErodedEdges(std::vector<sf::Vector2i> openFaces, spatial::Grid<CellState>& spaces, unsigned int seed) const;
		void reserveMachineBounds(spatial::Grid<CellState>& spaces, sf::IntRect bounds) const;

	private:
		unsigned int _seed = 0;

		using NoisePtr = std::unique_ptr<rng::PerlinNoise>;
		NoisePtr _temperatureNoise;
		NoisePtr _altitudeNoise;
		NoisePtr _moistureNoise;

		std::unordered_map<BiomeType, Biome> _biomes;
		mutable std::map<std::pair<int, int>, BiomeType> _cachedBiomeTypes;

		MachineFactory _machineFactory;
	};
}


