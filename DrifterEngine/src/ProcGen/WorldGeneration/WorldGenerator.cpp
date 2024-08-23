#include "pch.h"
#include "WorldGenerator.h"
#include "Algorithms/FloodFill.h"
#include "Factory/EntityFactory.h"
#include <Factory/Factory.h>
#include "Spatial/Conversions.h"
#include "Spatial/Helpers.h"
#include "Spatial/Grid.h"
#include "Structures/StructureInstance.h"
#include "Random/RandomNoise.h"
#include "Random/PercentChance.h"
#include "ProcGen/PlaceEntities.h"
#include "Services/DebugInfo.h"
#include "Systems/Helpers/GetCurrentCamera.h"
#include "Utility/stdHashing.h"
#include "Utility/Math.h"
#include "Spatial/WorldMapPosition.h"

#include <ProcGen/Layers/GenericLayers/PerlinNoiseLayer.h>
#include <ProcGen/Layers/EntityLayer.h>
#include <ProcGen/Layers/BiomeLayer.h>
#include <ProcGen/Layers/StructureLayer.h>
#include <ProcGen/Layers/JitteredGridLayer.h>
#include <ProcGen/Layers/VoronoiLayer.h>
#include <ProcGen/Layers/LandLayer.h>

static const sf::Vector2i CHUNK_SIZE = { drft::spatial::CHUNK_WIDTH, drft::spatial::CHUNK_HEIGHT };

using namespace entt::literals;

drft::gen::WorldGenerator::WorldGenerator()
{

}

void drft::gen::WorldGenerator::init()
{

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

		_dimensions = worldMapPosition.toChunkSpace();
	}
	if (json.HasMember("layers"))
	{
		for (auto&& layer : json["layers"].GetArray())
		{
			auto layerObj = layer.GetObject();
			const entt::id_type type = entt::hashed_string{ layerObj["type"].GetString() };

			if (type == "perlin"_hs)
			{
				entt::id_type id = entt::hashed_string{ layerObj["id"].GetString() };
				auto layerPtr = std::make_unique<PerlinNoiseLayer>(spatial::toTileSpace(_dimensions), _seed);
				auto& params = layerObj["params"];
				layerPtr->createFromJson(params);
				_layerManager->add(std::move(layerPtr), id);
			}
			else if (type == "land_layer"_hs)
			{
				auto layerPtr = std::make_unique<LandLayer>();
				auto& params = layerObj["params"];
				layerPtr->createFromJson(params);
				_layerManager->add(std::move(layerPtr));
			}
			else if (type == "voronoi"_hs)
			{
				auto layerPtr = std::make_unique<VoronoiLayer>();
				_layerManager->add(std::move(layerPtr));
			}
			else if (type == "jittered_grid"_hs)
			{
				auto layerPtr = std::make_unique<JitteredGridLayer>();
				_layerManager->add(std::move(layerPtr));
			}
		}
	}
}

void drft::gen::WorldGenerator::generate()
{
	// Generate any global layers

}

GenerationState drft::gen::WorldGenerator::generateChunk(sf::Vector2i coordinate, entt::registry& registry)
{
	sf::IntRect area = { spatial::toTileSpace(coordinate), CHUNK_SIZE };

	auto layer = _layerManager->generate<LandLayer>(area);
	if (layer.state != GenerationState::Complete) return layer.state;

	if (layer.instance)
	{
		const auto& factory = registry.ctx().get<EntityFactory&>();
		spatial::forEachPointInRect(area, [&factory, &layer, &registry](sf::Vector2i point)
			{
				if (layer.instance->isLand(point))
				{
					placeSingle("Sand", point, registry, factory);
				}
				else
				{
					placeSingle("Water", point, registry, factory);
				}
			});
	}
	return layer.state;
}

sf::Vector2i drft::gen::WorldGenerator::getDimensions() const
{
	return _dimensions;
}



