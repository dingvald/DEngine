#pragma once
#include <ProcGen/LayeredProcGen/LayeredProcGen.h>
#include <Biomes/BiomeRegistry.h>
#include <Spatial/AutoGrid.h>
#include <tuple>

#include <JSON/ICreateFromJson.h>

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
	void assignBiomeToVoronoiCell(sf::Vector2i centroid, BiomeCentroids& biomeCentroids, const ClimateValues& climateValues);
	ClimateValues getClimateValuesAtPoint(sf::Vector2i point, const std::unordered_map<entt::id_type, IGetValueAt*>& generatedDependencies) const;
	virtual int numLevels() override { return 2; }

	GenerationState assignBiomesToVoronoiCells(sf::IntRect area);
	GenerationState generateBiomeSlots(sf::IntRect area);

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
	BiomeCentroids getBiomeCentroidsInBounds(sf::IntRect area);
	std::vector<BiomeSlotPoint> getBiomeEntitySlotPointsInBounds(sf::IntRect area);
	
private:


private:
	BiomeRegistry _biomes;
	std::unordered_set<entt::id_type> _climateDependencies;
};