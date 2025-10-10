#pragma once
#include <Biomes/BiomeRegistry.h>
#include <Generation/Features/FeatureRegistry.h>
#include <Generation/EntityPack/EntityPackRegistry.h>
#include <Generation/LayerPack/LayerPackRegistry.h>
#include <Generation/Prefabs/PrefabRegistry.h>
#include <Generation/Structures/StructureRegistry.h>
#include <Factory/EntityFactory.h>
#include <Generation/PositionGenerator/PositionGeneratorFactory.h>

struct GenerationRegistries
{
	drft::EntityFactory entityFactory;
	BiomeRegistry biomes;
	FeatureRegistry features;
	EntityPackRegistry entityPacks;
	LayerPackRegistry layerPacks;
	PrefabRegistry prefabs;
	StructureRegistry structures;
	PositionGeneratorFactory positionGenerators;
};