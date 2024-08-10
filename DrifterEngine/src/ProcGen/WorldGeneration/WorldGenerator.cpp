#include "pch.h"
#include "WorldGenerator.h"
#include "Algorithms/FloodFill.h"
#include "Factory/EntityFactory.h"
#include "Spatial/Conversions.h"
#include "Spatial/Helpers.h"
#include "Spatial/Grid.h"
#include "Structures/StructureInstance.h"
#include "JSON/JSONHelpers.h"
#include "Random/RandomNumberGenerator.h"
#include "Random/RandomNoise.h"
#include "Random/NoiseLayer.h"
#include "Random/PercentChance.h"
#include "Random/WeightedSelection.h"
#include "ProcGen/GenerationContext.h"
#include "ProcGen/PlaceEntities.h"
#include "Services/DebugInfo.h"
#include "Systems/Helpers/GetCurrentCamera.h"
#include "Utility/stdHashing.h"
#include "Utility/Math.h"
#include "Spatial/WorldMapPosition.h"

#include <ProcGen/Layers/EntityLayer.h>
#include <ProcGen/Layers/BiomeLayer.h>
#include <ProcGen/Layers/StructureLayer.h>

static const sf::Vector2i CHUNK_SIZE = { drft::spatial::CHUNK_WIDTH, drft::spatial::CHUNK_HEIGHT };

static const std::filesystem::path STATIC_DATA_PATH = ".\\data\\static\\";
static const std::filesystem::path BIOME_FOLDER_PATH = STATIC_DATA_PATH.string() + "biomes";
static const std::filesystem::path STRUCTURE_FOLDER_PATH = STATIC_DATA_PATH.string() + "structures";

constexpr int GENERATION_PASSES = 3;

using namespace entt::literals;

drft::gen::WorldGenerator::WorldGenerator()
{
	
}

void drft::gen::WorldGenerator::init()
{
	_layerManager = std::make_unique<GenerationLayerManager>();
	_layerManager->add<EntityLayer>();
	_layerManager->add<BiomeLayer>();
	_layerManager->add<StructureLayer>();
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
			rng::RandomNumberGenerator::setSeed(_seed);
		}
		if (worldSettings.HasMember("Dimensions"))
		{
			const int raw_x = worldSettings["Dimensions"].GetArray()[0].GetInt();
			const int raw_y = worldSettings["Dimensions"].GetArray()[1].GetInt();
			const auto worldMapPosition = WorldMapPosition{ raw_x, raw_y };
			
			_dimensions = worldMapPosition.toChunkSpace();
		}
		if (worldSettings.HasMember("NoiseLayers"))
		{

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

	GenerationState state =  _layerManager->generate<EntityLayer>(area);
	if (state == GenerationState::Complete)
	{
		auto& layer = _layerManager->get<EntityLayer>();
		auto entityPositions = layer.getEntitiesInBounds(area);
		auto& factory = registry.ctx().get<EntityFactory&>();
		for (auto&& [position, entities] : entityPositions)
		{
			for (auto&& entity : entities)
			{
				placeSingle(entity, position, registry, factory);
			}
		}
	}

	return state;
}

sf::Vector2i drft::gen::WorldGenerator::getDimensions() const
{
	return _dimensions;
}

