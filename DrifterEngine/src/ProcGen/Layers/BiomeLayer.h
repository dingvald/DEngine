#pragma once
#include <ProcGen/LayeredProcGen/LayeredProcGen.h>
#include <Biomes/BiomeRegistry.h>
#include <Spatial/AutoGrid.h>
#include <tuple>

#include <JSON/ICreateFromJson.h>

namespace drft
{
	class BiomeLayer;

	using BiomeCentroids = std::unordered_map<sf::Vector2i, const Biome*>;
	using ClimateValues = std::unordered_map<entt::id_type, float>;

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
		void assignBiomeToVoronoiCell(sf::Vector3i centroid, BiomeCentroids& biomeCentroids, const ClimateValues& climateValues);
		ClimateValues getClimateValuesAtPoint(sf::Vector3i point, const std::unordered_map<entt::id_type, OnDemandLayer*>& generatedDependencies) const;
		virtual int numLevels() const override { return 2; }

		GenerationState assignBiomesToVoronoiCells(spatial::AABB<int> volume);
		GenerationState generateBiomeSlots(spatial::AABB<int> volume);

	public:
		std::vector<sf::Vector2i> biomePositions;
		BiomeCentroids biomePoints;
		std::vector<BiomeSlotPoint> biomeSlotPoints;
	};

	class BiomeLayer : public GenerationLayer<BiomeLayer, BiomeLayerChunk>, public ICreateFromJson
	{
	public:
		BiomeLayer(const BiomeRegistry& biomeRegistry);

		void createFromJson(const rapidjson::Value& json) override;

		const std::vector<const Biome*>& getBiomes() const;
		const std::unordered_set<entt::id_type>& getClimateDependencies() const;
		// Gets the Biome Entity Slot Points within a given area and origin (z-level)
		// Requires level 2 generation
		std::vector<BiomeSlotPoint> getBiomeEntitySlotPointsInArea(sf::IntRect area, sf::Vector3i origin);
		// Gets the Biome at a given tile positions
		// Requires level 1 generation
		const Biome* getBiomeAt(sf::Vector3i tilePosition) const;

	private:
		const BiomeRegistry& _biomeRegistry;
		std::vector<const Biome*> _biomes;
		std::unordered_set<entt::id_type> _climateDependencies;
	};
}

