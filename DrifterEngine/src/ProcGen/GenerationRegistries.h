#pragma once
#include <Biomes/BiomeRegistry.h>
#include <ProcGen/Features/FeatureRegistry.h>
#include <ProcGen/EntityPack/EntityPackRegistry.h>
#include <ProcGen/LayerPack/LayerPackRegistry.h>
#include <ProcGen/Structures/PrefabStructureRegistry.h>
#include <Factory/EntityFactory.h>

struct GenerationRegistries
{
	drft::EntityFactory entityFactory;
	BiomeRegistry biomes;
	FeatureRegistry features;
	EntityPackRegistry entityPacks;
	LayerPackRegistry layerPacks;
	PrefabStructureRegistry structures;
};