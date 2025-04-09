#pragma once
#include <ProcGen/LayeredProcGen/LayeredProcGen.h>
#include <Biomes/BiomeRegistry.h>
#include <Biomes/Features/BiomeFeatureRegistry.h>

#include <JSON/ICreateFromJson.h>

namespace drft
{
	class BiomeLayer;

	using BiomeCentroids = std::unordered_map<sf::Vector2i, const Biome*>;
	using DependencyValues = std::unordered_map<entt::id_type, float>;

	struct BiomeSlotPoint
	{
		const Biome* biome;
		entt::id_type slotID;
		sf::Vector2i point;
	};

	class BiomeLayerChunk : public GenerationChunk<BiomeLayer, BiomeLayerChunk>
	{
	public:
		using GenerationChunk::GenerationChunk;
		virtual GenerationState generate(int level) override;

	private:
		void assignBiomeToVoronoiCell(sf::Vector3i centroid, const DependencyValues& climateValues);
		DependencyValues getClimateValuesAtPoint(sf::Vector3i point, const std::unordered_map<entt::id_type, IGetValueAtLayer*>& generatedDependencies) const;
		virtual int numLevels() const override { return 2; }

		GenerationState stage1_assignBiomesToVoronoiCells(spatial::AABB<int> volume);
		GenerationState stage2_generateBiomeFeatures(spatial::AABB<int> volume);
		GenerationState stage3_generateBiomeSlots(spatial::AABB<int> volume);

	public:
		std::vector<sf::Vector2i> biomePositions;
		BiomeCentroids biomePoints;
		std::vector<BiomeSlotPoint> biomeSlotPoints;
	};

	class BiomeLayer : public GenerationLayer<BiomeLayer, BiomeLayerChunk>, public ICreateFromJson
	{
	public:
		BiomeLayer(const BiomeRegistry& biomeRegistry, const BiomeFeatureRegistry& featureRegistry);

		void createFromJson(const rapidjson::Value& json) override;

		const std::vector<const Biome*>& getBiomes() const;
		const BiomeFeatureRegistry& getFeatureRegistry() const;
		const std::unordered_set<entt::id_type>& getClimateDependencies() const;
		// Gets the Biome Entity Slot Points within a given area and origin (z-level)
		// Requires level 3 generation
		std::vector<BiomeSlotPoint> getBiomeEntitySlotPointsInArea(sf::IntRect area, sf::Vector3i origin);
		// Gets the Biome at a given tile positions
		// Requires level 1 generation
		const Biome* getBiomeAt(sf::Vector3i tilePosition) const;

	private:
		const BiomeRegistry& _biomeRegistry;
		const BiomeFeatureRegistry& _featureRegistry;
		std::vector<const Biome*> _biomes;
		std::unordered_set<entt::id_type> _climateDependencies;
	};
}

