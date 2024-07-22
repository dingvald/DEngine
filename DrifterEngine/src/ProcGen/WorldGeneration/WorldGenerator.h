#pragma once
#include "Random/NoiseLayer.h"
#include "Spatial/Grid.h"
#include "Spatial/AutoGrid.h"
#include "Utility/stdHashing.h"
#include "Structures/StructureFactory.h"
#include "Biomes/BiomeRegistry.h"
#include "Biomes/BiomeZone.h"

namespace drft
{
	class EntityFactory;
}

namespace drft::gen
{
	struct GenerationContext;

	enum class GenerationStatus
	{
		Start,
		Continue,
		Done
	};

	class WorldGenerator
	{
	public:
		WorldGenerator();
		void init();
		void createWorldMapSettingsFromJSON(const std::string& JSONfilename);
		// Generates the entire world other than chunks.
		void generate();

		GenerationStatus generateChunk(sf::Vector2i coordinate, entt::registry& registry) const;

		// Places the generated entities for this chunk into the registry.

		sf::Vector2i getStartingPosition(const std::string& biomeType) const;
		BiomeIcon getBiomeIcon(sf::Vector2i coordinate) const;
		sf::Vector2i getDimensions() const;

		void fixedUpdate(const entt::registry& registry);

		template<class Archive>
		void load(Archive& iarchive);
		template<class Archive>
		void save(Archive& oarchive) const;

	private:
		struct GenerationProgress
		{
			int pass = 0;
			EntityPositionMap entities;
		};


		void generateTerrain();
		void removeIsolatedBiomes();
		void generateZones();
		void fillBiomeMap();
		double getPerlinAt(const std::string& mapType, sf::Vector2i coordinate) const;
		float getRangeFromPerlin(const std::string& mapName, double perlinValue) const;
		const Biome* determineBiome(sf::Vector2i tilePosition) const;
		void placeStructures(GenerationContext& context, const Biome* biomeType) const;
		void generateSubChunk(sf::Vector2i subChunkCoordinate, GenerationContext& context, int passNum) const;
		void finalizeChunk(sf::Vector2i coordinate, entt::registry& registry) const;
		void placeTile(sf::Vector2i position, GenerationContext& context) const;
		void placeLiquid(sf::Vector2i position, GenerationContext& context) const;
		void placeEntities(const EntityPositionMap& entities, entt::registry& registry) const;
		void updateCompletedChunks(sf::Vector2i coordinate) const;
		sf::IntRect determinePlacementArea(sf::Vector2i coordinate) const;
		void initializeGlobalRanges();
		void clearSubchunkCache(sf::Vector2i chunkCoordinate) const;

	private:
		using NoiseMap = spatial::Grid<double>;
		using TagGrid = spatial::AutoGrid < entt::dense_set<entt::id_type> >;
		using TagGridPtr = std::unique_ptr<TagGrid>;

		unsigned int _seed = 0;
		sf::Vector2i _dimensions;
		spatial::Grid<const Biome*> _biomeMap;
		StructureFactory _structureFactory;
		BiomeRegistry _biomeRegistry;
		std::unordered_map<sf::Vector2i, sf::IntRect> _globalStructures;
		std::unordered_map<unsigned int, BiomeZone> _zones;
		std::unordered_map<std::string, rng::NoiseLayer> _noiseLayers;
		std::unordered_map<std::string, drft::math::Range<float>> _globalRanges;

		mutable std::unordered_map<sf::Vector2i, int> _completedChunks;
		mutable std::unordered_map<sf::Vector2i, GenerationProgress> _currentChunkGenerations;
		mutable std::unordered_map<sf::Vector2i, const Biome*> _subchunkBiomeCache;
		mutable TagGridPtr _tagGrid;
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


