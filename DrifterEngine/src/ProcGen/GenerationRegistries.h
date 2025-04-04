#include <Biomes/BiomeRegistry.h>
#include <ProcGen/EntityPack/EntityPackRegistry.h>
#include <ProcGen/LayerPack/LayerPackRegistry.h>
#include <ProcGen/Layers/LayerFactory.h>

struct GenerationRegistries
{
	BiomeRegistry biomes;
	EntityPackRegistry entityPacks;
	LayerPackRegistry layerPacks;

	LayerFactory layerFactory;
};