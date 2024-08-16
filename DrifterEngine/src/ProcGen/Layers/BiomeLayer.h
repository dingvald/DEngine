#pragma once
#include <ProcGen/LayeredProcGen/LayeredProcGen.h>
#include <Biomes/BiomeRegistry.h>
#include <Spatial/Grid.h>

class BiomeLayer;

class BiomeLayerChunk : public GenerationChunk<BiomeLayer, BiomeLayerChunk>
{
public:
	using GenerationChunk::GenerationChunk;
	virtual GenerationState generate() override;

private:
	drft::spatial::Grid<Biome*> _biomeMap;
};



class BiomeLayer : public GenerationLayer<BiomeLayer, BiomeLayerChunk>
{
public:
	BiomeLayer();

	const std::unordered_set<std::string>& getClimateTypes() const;

private:
	BiomeRegistry _biomes;
	std::unordered_set<std::string> _biomeClimateTypes;
};