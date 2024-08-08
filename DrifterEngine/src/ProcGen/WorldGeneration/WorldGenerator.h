#pragma once
#include "Random/NoiseLayer.h"
#include "Spatial/Grid.h"
#include "Spatial/AutoGrid.h"
#include "Utility/stdHashing.h"
#include "Structures/StructureManager.h"
#include "Biomes/BiomeRegistry.h"
#include "Biomes/BiomeZone.h"

#include <ProcGen/GenerationLayer/GenerationLayerManager.h>

namespace drft
{
	class EntityFactory;
}

namespace drft::gen
{
	struct GenerationContext;

	class WorldGenerator
	{
	public:
		WorldGenerator();
		void init();
		void createFromJson(const std::string& JSONfilename);
		// Generates the entire world other than chunks.
		void generate();

		GenerationState generateChunk(sf::Vector2i coordinate, entt::registry& registry);

		const Biome* getBiome(sf::Vector2i coordinate) const;

		sf::Vector2i getStartingPosition(const std::string& biomeType) const;
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
		const Biome* determineBiome(sf::Vector2i tilePosition) const;

		void placeTile(sf::Vector2i position, GenerationContext& context) const;
		void placeLiquid(sf::Vector2i position, GenerationContext& context) const;
		void generateEntities(sf::Vector2i position, int pass, GenerationContext& context, const Biome* biome) const;
		void placeEntities(const EntityPositionMap& entities, entt::registry& registry) const;
		sf::IntRect determinePlacementArea(sf::Vector2i coordinate) const;
		void initializeGlobalRanges();

	private:
		using NoiseMap = spatial::Grid<double>;
		using TagGrid = spatial::AutoGrid < entt::dense_set<entt::id_type> >;
		using TagGridPtr = std::unique_ptr<TagGrid>;

		unsigned int _seed = 0;
		sf::Vector2i _dimensions;

		GenerationLayerManager _layerManager;

		spatial::Grid<const Biome*> _biomeMap;
		StructureManager _structureManager{*this};
		BiomeRegistry _biomeRegistry;

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


