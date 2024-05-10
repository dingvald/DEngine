#pragma once
#include "Random/NoiseLayer.h"
#include "Spatial/Grid.h"
#include "Spatial/AutoGrid.h"
#include "Utility/stdHashing.h"
#include "Structures/StructureFactory.h"
#include "Biomes/BiomeRegistry.h"
#include "Biomes/BiomeZone.h"
#include "ProcGen/SpawningAlgorithms/SpawningAlgorithmRegistry.h"

namespace drft::gen
{
	struct GenerationContext;

	class WorldGenerator
	{
	public:
		WorldGenerator();
		void init();
		void createWorldMapSettingsFromJSON(const std::string& JSONfilename);
		// Generates the entire world other than chunks.
		void generate();
		// Generates the concrete chunk.
		void finalizeChunk(sf::Vector2i coordinate, entt::registry& registry) const;

		sf::Vector2i getStartingPosition(const std::string& biomeType) const;
		BiomeIcon getBiomeIcon(sf::Vector2i coordinate) const;
		sf::Vector2i getDimensions() const;

		void fixedUpdate(const entt::registry& registry);

		template<class Archive>
		void load(Archive& iarchive);
		template<class Archive>
		void save(Archive& oarchive) const;

	private:
		void generateTerrain();
		void removeIsolatedBiomes();
		void generateZones();
		void fillBiomeMap();
		double getPerlinAt(const std::string& mapType, sf::Vector2i coordinate) const;
		float getRangeFromPerlin(const std::string& mapName, double perlinValue) const;
		const Biome* determineBiome(sf::Vector2i coordinate) const;
		void placeStructures(const GenerationContext& context, const Biome* biomeType, entt::registry& registry) const;
		void placeLiquids(const GenerationContext& context, const Biome* biomeType, entt::registry& registry) const;
		void placeEntities(const GenerationContext& context, const Biome* biomeType, entt::registry& registry) const;
		void updateCompletedChunks(sf::Vector2i coordinate) const;
		sf::IntRect determinePlacementArea(sf::Vector2i coordinate) const;
		void initializeGlobalRanges();

	private:
		using NoiseMap = spatial::Grid<double>;
		using BitGridPtr = std::unique_ptr< spatial::AutoGrid<std::bitset<32>>>;

		unsigned int _seed = 0;
		sf::Vector2i _dimensions;
		mutable std::unordered_map<sf::Vector2i, int> _completedChunks;
		spatial::Grid<const Biome*> _biomeMap;
		StructureFactory _structureFactory;
		BiomeRegistry _biomeRegistry;
		SpawningAlgorithmRegistry _spawningAlgorithms;
		std::unordered_map<sf::Vector2i, sf::IntRect> _globalStructures;
		mutable BitGridPtr _bitGrid;
		std::unordered_map<unsigned int, BiomeZone> _zones;
		std::unordered_map<std::string, rng::NoiseLayer> _noiseLayers;
		std::unordered_map<std::string, drft::math::Range<float>> _globalRanges;
	};

	template<class Archive>
	inline void WorldGenerator::load(Archive& archive)
	{
		archive(cereal::make_nvp("Seed", _seed));
		archive(cereal::make_nvp("Width", _dimensions.x));
		archive(cereal::make_nvp("Height", _dimensions.y));
		archive(_noiseLayers);
	}
	template<class Archive>
	inline void WorldGenerator::save(Archive& archive) const
	{
		archive(cereal::make_nvp("Seed", _seed));
		archive(cereal::make_nvp("Width", _dimensions.x));
		archive(cereal::make_nvp("Height", _dimensions.y));
		archive(_noiseLayers);
	}
}


