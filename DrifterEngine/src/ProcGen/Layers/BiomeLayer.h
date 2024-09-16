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
		ClimateValues getClimateValuesAtPoint(sf::Vector3i point, const std::unordered_map<entt::id_type, IGetValueAt*>& generatedDependencies) const;
		virtual int numLevels() const override { return 2; }

		GenerationState assignBiomesToVoronoiCells(spatial::AABB<int> volume);
		GenerationState generateBiomeSlots(spatial::AABB<int> volume);

	public:
		BiomeCentroids biomePoints;
		std::vector<BiomeSlotPoint> biomeSlotPoints;
	};



	class BiomeLayer : public GenerationLayer<BiomeLayer, BiomeLayerChunk>
	{
	public:
		BiomeLayer();

		const BiomeRegistry& getBiomeRegistry() const;
		const std::unordered_set<entt::id_type>& getClimateDependencies() const;
		BiomeCentroids getBiomeCentroidsInArea(sf::IntRect area, sf::Vector3i origin);
		std::vector<BiomeSlotPoint> getBiomeEntitySlotPointsInArea(sf::IntRect area, sf::Vector3i origin);
		const Biome* getBiomeAt(sf::Vector3i tilePosition) const;

	private:


	private:
		BiomeRegistry _biomes;
		std::unordered_set<entt::id_type> _climateDependencies;
	};
}

