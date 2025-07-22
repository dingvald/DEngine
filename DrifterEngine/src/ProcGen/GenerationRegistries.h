#pragma once
#include <Biomes/BiomeRegistry.h>
#include <ProcGen/Features/FeatureRegistry.h>
#include <ProcGen/EntityPack/EntityPackRegistry.h>
#include <ProcGen/LayerPack/LayerPackRegistry.h>
#include <ProcGen/Prefabs/PrefabRegistry.h>
#include <ProcGen/Structures/StructureRegistry.h>
#include <Factory/EntityFactory.h>

struct GenerationRegistries
{
	drft::EntityFactory entityFactory;
	BiomeRegistry biomes;
	FeatureRegistry features;
	EntityPackRegistry entityPacks;
	LayerPackRegistry layerPacks;
	PrefabRegistry prefabs;
	StructureRegistry structures;
};