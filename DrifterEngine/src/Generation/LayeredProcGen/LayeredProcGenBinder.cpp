#include "pch.h"
#include "LayeredProcGenBinder.h"
#include <Generation/LayeredProcGen/LayeredProcGen.h>

#include <Generation/Layers/VoronoiLayer.h>
#include <Generation/Layers/JitteredGridLayer.h>
#include <Generation/Layers/RandomLayer.h>
#include <Generation/Layers/PerlinNoiseLayer.h>
#include <Generation/Layers/PositionArrayLayer.h>
#include <Generation/Layers/LloydRelaxedLayer.h>
#include <Generation/Layers/FillLayer.h>
#include <Generation/Layers/DepthLayer.h>
#include <Generation/Layers/FeatureLayer.h>
#include <Generation/Layers/EntitySlotLayer.h>
#include <Generation/Layers/EntityPlacementLayer.h>
#include <Generation/Layers/TilePlacementLayer.h>

void drft::LayeredProcGenBinder::bindLayers(GenerationLayerManager& layers)
{
	using namespace entt::literals;

	// register layer types that can have multiple instances created using add
	layers.registerType<drft::RandomLayer>("random"_hs);
	layers.registerType<drft::PerlinNoiseLayer>("perlin"_hs);
	layers.registerType<drft::LloydRelaxedLayer>("relaxed_points"_hs);
	layers.registerType<drft::FillLayer>("fill"_hs);
	layers.registerType<drft::DepthLayer>("depth"_hs);
	layers.registerType<drft::PositionArrayLayer>("position_array"_hs);

	// add layers that are used by all managers
	layers.add<drft::RandomLayer>();
	layers.add<drft::JitteredGridLayer>();
	layers.add<drft::VoronoiLayer>();
	layers.add<drft::FeatureLayer>();
	layers.add<drft::EntitySlotLayer>();
	layers.add<drft::EntityPlacementLayer>();
	layers.add<drft::TilePlacementLayer>();
}
