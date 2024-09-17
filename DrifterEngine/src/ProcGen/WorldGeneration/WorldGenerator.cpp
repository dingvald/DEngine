#include "pch.h"
#include "WorldGenerator.h"
#include "Algorithms/FloodFill.h"
#include "Factory/EntityFactory.h"
#include <Factory/Factory.h>
#include "Spatial/Conversions.h"
#include "Spatial/Helpers.h"
#include "Structures/StructureInstance.h"
#include "Random/RandomNoise.h"
#include "Random/PercentChance.h"
#include "ProcGen/PlaceEntities.h"
#include "Services/DebugInfo.h"
#include "Systems/Helpers/GetCurrentCamera.h"
#include "Utility/ContainerHelpers.h"
#include "Utility/stdHashing.h"
#include "Utility/Math.h"
#include "Spatial/WorldMapPosition.h"

#include <ProcGen/Layers/PerlinNoiseLayer.h>
#include <ProcGen/Layers/BiomeLayer.h>
#include <ProcGen/Layers/JitteredGridLayer.h>
#include <ProcGen/Layers/LloydRelaxedLayer.h>
#include <ProcGen/Layers/RandomLayer.h>
#include <ProcGen/Layers/VoronoiLayer.h>
#include <ProcGen/Layers/FillLayer.h>

using namespace entt::literals;

using namespace entt::literals;

void drft::gen::WorldGenerator::init()
{
	// Manually add layers that aren't included in the json
	_layerManager->add(std::make_unique<VoronoiLayer>());
	_layerManager->add(std::make_unique<JitteredGridLayer>());
	_layerManager->add(std::make_unique<FillLayer>());
	_layerManager->add(std::make_unique<BiomeLayer>());
}

void drft::gen::WorldGenerator::createFromJson(const rapidjson::Value& json)
{
	if (json.HasMember("seed"))
	{
		auto& seed = json["seed"];
		if (seed.IsString())
		{
			if (std::strcmp(seed.GetString(), "random") != 0)
			{
				throw std::exception("Invalid string.");
			}
			_seed = rng::generateSeed();
		}
		else if (seed.IsUint())
		{
			_seed = seed.GetUint();
		}
		rng::GlobalSeed = _seed;
		_layerManager = std::make_unique<GenerationLayerManager>(_seed);
	}
	if (json.HasMember("dimensions"))
	{
		const int raw_x = json["dimensions"].GetArray()[0].GetInt();
		const int raw_y = json["dimensions"].GetArray()[1].GetInt();
		const auto worldMapPosition = WorldMapPosition{ raw_x, raw_y };

		_dimensions = spatial::toChunkSpace(worldMapPosition);
	}
	if (json.HasMember("layers"))
	{
		for (auto&& [name, layer] : json["layers"].GetObject())
		{
			auto layerObj = layer.GetObject();
			const entt::id_type type = entt::hashed_string{ layerObj["type"].GetString() };
			entt::id_type id = entt::hashed_string{ name.GetString() };

			if (type == "perlin"_hs)
			{
				auto layerPtr = std::make_unique<PerlinNoiseLayer>(spatial::toXY(spatial::toTileSpace(_dimensions)), _seed);
				auto& params = layerObj["params"];
				layerPtr->createFromJson(params);
				_layerManager->add(std::move(layerPtr), id);
			}
			else if (type == "relaxed"_hs)
			{
				auto layerPtr = std::make_unique<LloydRelaxedLayer>();
				auto& params = layerObj["params"];
				layerPtr->createFromJson(params);
				_layerManager->add(std::move(layerPtr), id);
			}
			else if (type == "random"_hs)
			{
				auto layerPtr = std::make_unique<RandomLayer>();
				auto& params = layerObj["params"];
				layerPtr->createFromJson(params);
				_layerManager->add(std::move(layerPtr), id);
			}
		}
	}
}

void drft::gen::WorldGenerator::generate()
{
	// Generate any global layers

}

drft::GenerationState drft::gen::WorldGenerator::generateChunk(ChunkPosition coordinate, entt::registry& registry)
{
	const sf::Vector3i origin = spatial::toTileSpace(coordinate);
	const sf::Vector3i dimensions = spatial::asTileSpace(ChunkDimensions);
	spatial::AABB<int> volume = { origin, dimensions };

	auto layer = _layerManager->generate<BiomeLayer>(volume);
	if (!layer.isReady()) return layer.getState();

	rng::Random random{ _seed + std::hash<ChunkPosition>()(coordinate) };
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
			auto tileHandle = placeSingle("Tile", spatial::asTileSpace(point3d), registry, factory);
			tileHandle.patch<RenderComponent>([&tileColor](RenderComponent& comp) {comp.color = tileColor; });
		});

	auto bsps = layer.unwrap().getBiomeEntitySlotPointsInArea(volume.flatten(), volume.min);
	for (auto&& [biome, slot, point] : bsps)
	{
		if (auto* entityPack = biome->getEntityPack(slot))
		{
			auto optionalSelection = random.weightedSelection(*entityPack);
			if (!optionalSelection.has_value()) continue;

			auto&& [entity, _] = entityPack->at(optionalSelection.value());
			placeSingle(entity, spatial::asTileSpace(sf::Vector3i{point.x, point.y, volume.min.z}), registry, factory);
		}
	}

	return GenerationState::Complete;
}

drft::ChunkPosition drft::gen::WorldGenerator::getDimensions() const
{
	return _dimensions;
}



