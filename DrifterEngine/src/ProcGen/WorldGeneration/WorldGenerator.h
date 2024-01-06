#pragma once
#include "Random/PerlinNoise.h"
#include "ProcGen/Biome.h"
#include "Spatial/Grid.h"
#include "Spatial/AutoGrid.h"
#include "Utility/stdHashing.h"
#include "Structures/StructureFactory.h"

namespace drft::gen
{
	class WorldGenerator
	{
	public:
		WorldGenerator();
		void init(sf::Vector2i dimensions, unsigned int seed);
		void loadBiomes(const std::string& JSONfilename);
		// Generates the entire world other than chunks.
		void generate();
		// Generates the concrete chunk.
		void finalizeChunk(sf::Vector2i coordinate, entt::registry& registry) const;
		sf::Vector2i getStartingPosition(std::string biomeType) const;
		BiomeIcon getBiomeIcon(sf::Vector2i coordinate) const;

	private:
		void generateTerrain();
		void removeIsolatedBiomes();
		void generateZones();
		std::unordered_set<sf::Vector2i> floodFillZone(sf::Vector2i startingNode, const BiomeType* type);
		double getPerlinAt(const std::string& mapType, sf::Vector2i coordinate) const;
		float getRangeFromPerlin(const std::string& mapName, double perlinValue) const;
		std::unordered_set<std::string> determinePotentialBiomes(sf::Vector2i coordinate) const;
		const BiomeType* selectBiomeType(sf::Vector2i coordinate) const;
		void blendBiomeBoundaries(sf::IntRect area, sf::Vector2i coordinate) const;
		void placeStructures(sf::IntRect area, const BiomeType* biomeType, entt::registry& registry) const;
		void placeEntities(sf::IntRect area, const BiomeType* biomeType, entt::registry& registry) const;
		void updateCompletedChunks(sf::Vector2i coordinate) const;
		sf::IntRect determinePlacementArea(sf::Vector2i coordinate) const;

	private:
		using NoiseMap = spatial::Grid<double>;
		using BitGridPtr = std::unique_ptr< spatial::AutoGrid<std::bitset<32>>>;

		unsigned int _seed = 0;
		sf::Vector2i _dimensions;
		std::unordered_map<std::string, BiomeType> _biomeTypes;
		mutable std::unordered_map<sf::Vector2i, int> _completedChunks;
		spatial::Grid<const BiomeType*> _biomeMap;
		std::unique_ptr<StructureFactory> _structureFactory;
		std::unordered_map<sf::Vector2i, sf::IntRect> _globalStructures;
		mutable BitGridPtr _bitGrid;
		std::unordered_map<unsigned int, BiomeZone> _zones;
		std::unordered_map<std::string, NoiseMap> _noiseMaps;
		std::unordered_map<std::string, Range> _ranges;
	};
}


