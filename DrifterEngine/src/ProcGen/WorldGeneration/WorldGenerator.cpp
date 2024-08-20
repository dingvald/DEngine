#include "pch.h"
#include "WorldGenerator.h"
#include "Algorithms/FloodFill.h"
#include "Factory/EntityFactory.h"
#include "Spatial/Conversions.h"
#include "Spatial/Helpers.h"
#include "Spatial/Grid.h"
#include "Structures/StructureInstance.h"
#include "JSON/JSONHelpers.h"
#include "Random/Random.h"
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

static const sf::Vector2i CHUNK_SIZE = { drft::spatial::CHUNK_WIDTH, drft::spatial::CHUNK_HEIGHT };

static const std::filesystem::path STATIC_DATA_PATH = ".\\data\\static\\";
static const std::filesystem::path BIOME_FOLDER_PATH = STATIC_DATA_PATH.string() + "biomes";
static const std::filesystem::path STRUCTURE_FOLDER_PATH = STATIC_DATA_PATH.string() + "structures";

using namespace entt::literals;

drft::gen::WorldGenerator::WorldGenerator()
{
	

}

void drft::gen::WorldGenerator::init()
{
	_layerManager->add(std::make_unique<EntityLayer>());
	_layerManager->add(std::make_unique<StructureLayer>());
	_layerManager->add(std::make_unique<BiomeLayer>());
	_layerManager->add(std::make_unique<JitteredGridLayer>());
	_layerManager->add(std::make_unique<VoronoiLayer>());
}

void drft::gen::WorldGenerator::createFromJson(const std::string& JSONfilename)
{
	std::filesystem::path worldSettingsFilePath;
	worldSettingsFilePath /= STATIC_DATA_PATH;
	worldSettingsFilePath /= JSONfilename;
	auto optionalWorldSettingsDocument = drft::json::extractDOM(worldSettingsFilePath, "WorldSettings");

	if (!optionalWorldSettingsDocument.has_value())
	{
		std::cout << "Failed: " << worldSettingsFilePath << " could not be parsed." << std::endl;
	}
	else
	{
		auto& worldSettingsDocument = optionalWorldSettingsDocument.value();
		auto& worldSettings = worldSettingsDocument["WorldSettings"];
		if (worldSettings.HasMember("Seed"))
		{
			auto& seed = worldSettings["Seed"];
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
		}
		if (worldSettings.HasMember("Dimensions"))
		{
			const int raw_x = worldSettings["Dimensions"].GetArray()[0].GetInt();
			const int raw_y = worldSettings["Dimensions"].GetArray()[1].GetInt();
			const auto worldMapPosition = WorldMapPosition{ raw_x, raw_y };
			
			_dimensions = worldMapPosition.toChunkSpace();
		}
		if (worldSettings.HasMember("Layers"))
		{
			_layerManager = std::make_unique<GenerationLayerManager>(_seed);

			for (auto&& layer : worldSettings["Layers"].GetArray())
			{
				auto layerObj = layer.GetObject();
				const std::string type = layerObj["type"].GetString();
				auto& params = layerObj["params"];
				if (type == "perlin")
				{
					entt::id_type id = entt::hashed_string{ layerObj["id"].GetString() };
					auto layerPtr = std::make_unique<PerlinNoiseLayer>();
					layerPtr->createFromJson(params);
					_layerManager->add(std::move(layerPtr), id);
				}
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

	auto layer = _layerManager->generate<VoronoiLayer>(area);
	if (layer.state != GenerationState::Complete) return layer.state;

	if (layer.instance)
	{
		const auto& factory = registry.ctx().get<EntityFactory&>();
		auto edges = layer.instance->getEdgesInBounds(area);
		for (auto&& edge : edges)
		{
			auto line = spatial::getIntPointsAlongLine(edge.first, edge.second);
			for (auto&& point : line)
			{
				placeSingle("Tile", point, registry, factory);
			}
		}
	}
	return layer.state;
}

sf::Vector2i drft::gen::WorldGenerator::getDimensions() const
{
	return _dimensions;
}

