#include "pch.h"
#include "CelestialBody.h"

#include <ProcGen/LayeredProcGen/LayeredProcGen.h>
#include "Algorithms/FloodFill.h"
#include "Factory/EntityFactory.h"
#include <Factory/Factory.h>
#include "Spatial/Conversions.h"
#include "Spatial/Helpers.h"
#include "Structures/StructureInstance.h"
#include "Random/RandomNoise.h"
#include <Random/Random.h>
#include "Random/PercentChance.h"
#include "ProcGen/PlaceEntities.h"
#include "Systems/Helpers/GetCurrentCamera.h"
#include "Utility/ContainerHelpers.h"
#include "Utility/stdHashing.h"
#include "Utility/Math.h"

#include <ProcGen/Layers/PerlinNoiseLayer.h>
#include <ProcGen/Layers/BiomeLayer.h>
#include <ProcGen/Layers/JitteredGridLayer.h>
#include <ProcGen/Layers/LloydRelaxedLayer.h>
#include <ProcGen/Layers/RandomLayer.h>
#include <ProcGen/Layers/VoronoiLayer.h>
#include <ProcGen/Layers/FillLayer.h>

using namespace drft;

GenerationState CelestialBody::generateChunk(drft::ChunkPosition position, entt::registry& registry)
{
	const sf::Vector3i origin = spatial::toTileSpace(position);
	const sf::Vector3i dimensions = spatial::asTileSpace(ChunkDimensions);
	spatial::AABB<int> volume = { origin, dimensions };

	auto layer = _layerManager.generate<BiomeLayer>(volume);
	if (!layer.isReady()) return layer.getState();

	rng::Random random{ +std::hash<ChunkPosition>()(position) };
	const auto& factory = registry.ctx().get<const EntityFactory&>();

	// Place tiles
	spatial::forEachPointInRect(volume.flatten(), [&registry, &factory, &layer, z = volume.min.z](sf::Vector2i point)
		{
			const sf::Vector3i point3d = { point.x, point.y, z };
			sf::Color tileColor = { 10,10,10 };
			if (auto biome = layer.unwrap().getBiomeAt(point3d))
			{
				tileColor = biome->getBaseTileColor();
			}
			auto tileHandle = gen::placeSingle("Tile", spatial::asTileSpace(point3d), registry, factory);
			tileHandle.patch<RenderComponent>([&tileColor](RenderComponent& comp) {comp.color = tileColor; });
		});

	auto bsps = layer.unwrap().getBiomeEntitySlotPointsInArea(volume.flatten(), volume.min);
	for (auto&& [biome, slot, point] : bsps)
	{
		if (auto* entityPack = biome->getEntityPack(slot))
		{
			if (auto optionalSelection = random.weightedSelection(*entityPack))
			{
				auto&& [entity, _] = entityPack->at(optionalSelection.value());
				gen::placeSingle(entity, spatial::asTileSpace(sf::Vector3i{ point.x, point.y, volume.min.z }), registry, factory);
			}
		}
	}

	return GenerationState::Complete;
}

entt::id_type CelestialBody::getSourceId()
{
    return entt::id_type();
}

void CelestialBody::tick()
{
}

void CelestialBody::createFromJson(const rapidjson::Value& json)
{
}
