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
#include <ProcGen/Layers/EntityLayer.h>
#include <ProcGen/Layers/BiomeLayer.h>
#include <ProcGen/Layers/StructureLayer.h>
#include <ProcGen/Layers/JitteredGridLayer.h>
#include <ProcGen/Layers/LloydRelaxedLayer.h>
#include <ProcGen/Layers/RandomLayer.h>
#include <ProcGen/Layers/VoronoiLayer.h>
#include <ProcGen/Layers/FillLayer.h>

static const sf::Vector2i CHUNK_SIZE = { drft::spatial::CHUNK_WIDTH, drft::spatial::CHUNK_HEIGHT };

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

GenerationState drft::gen::WorldGenerator::generateChunk(ChunkPosition coordinate, entt::registry& registry)
{
	const int z = spatial::toTileSpace(coordinate).z;
	const sf::Vector2i origin2d = spatial::toXY(spatial::toTileSpace(coordinate));
	const sf::Vector2i dimensions2d = spatial::toXY(spatial::asTileSpace(ChunkDimensions));
	sf::IntRect area = { origin2d, dimensions2d };

	auto layer = _layerManager->generate<BiomeLayer>(area);
	if (!layer.isReady()) return layer.getState();

	rng::Random random{ _seed + std::hash<ChunkPosition>()(coordinate) };
	const auto& factory = registry.ctx().get<const EntityFactory&>();

	spatial::forEachPointInRect(area, [&registry, &factory, &layer, &z](sf::Vector2i point)
		{
			sf::Color tileColor = { 10,10,10 };
			if (auto biome = layer.unwrap().getBiomeAt(point))
			{
				tileColor = biome->getBaseTileColor();
			}
			auto tileHandle = placeSingle("Tile", TilePosition{ point.x, point.y, z }, registry, factory);
			tileHandle.patch<RenderComponent>([&tileColor](RenderComponent& comp) {comp.color = tileColor; });
		});

	auto bsps = layer.unwrap().getBiomeEntitySlotPointsInBounds(area);
	for (auto&& [biome, slot, point] : bsps)
	{
		if (auto* entityPack = biome->getEntityPack(slot))
		{
			auto optionalSelection = random.weightedSelection(*entityPack);
			if (!optionalSelection.has_value()) continue;

			auto&& [entity, _] = entityPack->at(optionalSelection.value());
			placeSingle(entity, TilePosition{ point.x, point.y, z }, registry, factory);
		}
	}

	return GenerationState::Complete;
}

drft::ChunkPosition drft::gen::WorldGenerator::getDimensions() const
{
	return _dimensions;
}



