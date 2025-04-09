#include <Biomes/BiomeRegistry.h>
#include <Biomes/Features/BiomeFeatureRegistry.h>
#include <ProcGen/EntityPack/EntityPackRegistry.h>
#include <ProcGen/LayerPack/LayerPackRegistry.h>
#include <ProcGen/Layers/LayerFactory.h>

struct GenerationRegistries
{
	BiomeRegistry biomes;
	BiomeFeatureRegistry features;
	EntityPackRegistry entityPacks;
	LayerPackRegistry layerPacks;

	LayerFactory layerFactory;
};