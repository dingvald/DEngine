#pragma once
#include <Biomes/BiomeRegistry.h>
#include <Biomes/Features/BiomeFeatureRegistry.h>
#include <ProcGen/EntityPack/EntityPackRegistry.h>
#include <ProcGen/LayerPack/LayerPackRegistry.h>

struct GenerationRegistries
{
	BiomeRegistry biomes;
	BiomeFeatureRegistry features;
	EntityPackRegistry entityPacks;
	LayerPackRegistry layerPacks;
};