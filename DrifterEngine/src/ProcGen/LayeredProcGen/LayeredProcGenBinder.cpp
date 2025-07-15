#include "pch.h"
#include "LayeredProcGenBinder.h"
#include <ProcGen/LayeredProcGen/LayeredProcGen.h>

#include <ProcGen/Layers/VoronoiLayer.h>
#include <ProcGen/Layers/JitteredGridLayer.h>
#include <ProcGen/Layers/RandomLayer.h>
#include <ProcGen/Layers/PerlinNoiseLayer.h>
#include <ProcGen/Layers/PositionArrayLayer.h>
#include <ProcGen/Layers/LloydRelaxedLayer.h>
#include <ProcGen/Layers/FillLayer.h>
#include <ProcGen/Layers/DepthLayer.h>
#include <ProcGen/Layers/FeatureLayer.h>
#include <ProcGen/Layers/EntitySlotLayer.h>
#include <ProcGen/Layers/EntityPlacementLayer.h>
#include <ProcGen/Layers/TilePlacementLayer.h>

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
