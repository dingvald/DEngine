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

static const sf::Vector2i CHUNK_SIZE = { drft::spatial::CHUNK_WIDTH, drft::spatial::CHUNK_HEIGHT };

static const std::filesystem::path STATIC_DATA_PATH = ".\\data\\static\\";
static const std::filesystem::path BIOME_FOLDER_PATH = STATIC_DATA_PATH.string() + "biomes";
static const std::filesystem::path STRUCTURE_FOLDER_PATH = STATIC_DATA_PATH.string() + "structures";

constexpr int GENERATION_PASSES = 3;

using namespace entt::literals;

drft::gen::WorldGenerator::WorldGenerator()
{
	_tagGrid = std::make_unique<TagGrid>(CHUNK_SIZE.x, CHUNK_SIZE.y);
}

void drft::gen::WorldGenerator::init()
{

	_biomeMap.resize(_dimensions.x, _dimensions.y);

	_biomeRegistry.createBiomesFromJSON(BIOME_FOLDER_PATH);
	_structureFactory.createStructureBlueprintsFromJSON(STRUCTURE_FOLDER_PATH);

	initializeGlobalRanges();
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
			unsigned int currentSeed = _seed;
			for (auto&& noiseLayer : worldSettings["NoiseLayers"].GetObject())
			{
				rng::NoiseLayer layer = { spatial::toTileSpace(_dimensions), currentSeed };
				layer.createFromJSON(noiseLayer.value);
				_noiseLayers.emplace(noiseLayer.name.GetString(), layer);
				currentSeed = rng::noise(currentSeed);
			}
		}
	}
}

void drft::gen::WorldGenerator::generate()
{
	std::cout << "Generating terrain data..." << std::endl;
	generateTerrain();
	std::cout << "Terrain complete." << std::endl;
	std::cout << "Generating zones..." << std::endl;
	generateZones();
	std::cout << "Zones complete." << std::endl;
	// generate dungeons
	// generate modifications
	// generate structures

}

drft::gen::GenerationStatus drft::gen::WorldGenerator::generateChunk(sf::Vector2i coordinate, entt::registry& registry) const
{
	if (!_currentChunkGenerations.contains(coordinate))
	{
		_currentChunkGenerations.emplace(coordinate, GenerationProgress{});
	}

	const sf::IntRect placementArea = determinePlacementArea(coordinate);
	auto& entityPositions = _currentChunkGenerations.at(coordinate).entities;
	const int pass = _currentChunkGenerations.at(coordinate).pass;

	GenerationContext context = {
		.area = placementArea,
		.entityPositions = entityPositions,
		.grid = *_tagGrid,
		.noiseLayers = _noiseLayers,
		.seed = _seed
	};

	const Biome* biome = determineBiome(spatial::toTileSpace(coordinate));

	for (int y = placementArea.top; y < placementArea.top + placementArea.height; y++)
	{
		for (int x = placementArea.left; x < placementArea.left + placementArea.width; x++)
		{
			const sf::Vector2i position = { x, y };
			if (pass == 0)
			{
				placeTile(position, context);
				placeLiquid(position, context);
			}
			generateEntities(position, pass, context, biome);
		}
	}

	if (++_currentChunkGenerations.at(coordinate).pass >= GENERATION_PASSES)
	{
		finalizeChunk(coordinate, registry);
		return GenerationStatus::Done;
	}

	return GenerationStatus::Continue;
}

const Biome* drft::gen::WorldGenerator::getBiome(sf::Vector2i coordinate) const
{
	if (!_biomeMap.contains(coordinate.x, coordinate.y)) return nullptr;
	return _biomeMap.at(coordinate.x, coordinate.y);
}

void drft::gen::WorldGenerator::generateTerrain()
{
	// Add north pole
	_noiseLayers.at("Temperature").addBiasingFunction(
		[](double val, sf::Vector2i position)
		{
			double mod = std::clamp(0.0005 * position.y, 0.0, 1.1);
			return val * mod;
		});
	// Make Volcanism rarer
	_noiseLayers.at("Volcanism").addBiasingFunction(
		[](double val, sf::Vector2i position)
		{
			return val - 0.5;
		});

	fillBiomeMap();

	removeIsolatedBiomes();
	// Post process
}

void drft::gen::WorldGenerator::removeIsolatedBiomes()
{
	for (int y = 0; y < _dimensions.y; ++y)
	{
		for (int x = 0; x < _dimensions.x; ++x)
		{
			auto centerBiome = _biomeMap.at(x, y);
			bool isolated = true;
			auto surroundings = spatial::getAdjacentPoints({ x, y });
			std::unordered_map<const Biome*, int> count;
			std::vector<const Biome*> toShuffle;
			for (auto&& cell : surroundings)
			{
				if (!_biomeMap.contains(cell.x, cell.y)) continue;
				const Biome* neighbor = _biomeMap.at(cell.x, cell.y);
				if (neighbor == centerBiome)
				{
					isolated = false;
					break;
				}
				if (!count.contains(neighbor))
				{
					toShuffle.push_back(neighbor);
				}
				count[neighbor]++;
			}

			if (isolated)
			{
				std::shuffle(toShuffle.begin(), toShuffle.end(), std::default_random_engine(_seed));
				int largest = 0;
				const Biome* choice = nullptr;
				for (auto&& n : toShuffle)
				{
					if (count[n] > largest)
					{
						largest = count[n];
						choice = n;
					}
				}

				_biomeMap.at(x, y) = choice;
			}
		}
	}
}

void drft::gen::WorldGenerator::generateZones()
{
	std::unordered_set<sf::Vector2i> possibleSpaces;
	for (int y = 0; y < _dimensions.y; ++y)
	{
		for (int x = 0; x < _dimensions.x; ++x)
		{
			possibleSpaces.insert(sf::Vector2i(x, y));
		}
	}

	unsigned int zoneID = 1;
	while (!possibleSpaces.empty())
	{
		sf::Vector2i position = *possibleSpaces.begin();
		const Biome* workingType = _biomeMap.at(position.x, position.y);
		auto biomeTypeChecker = [workingType, this](sf::Vector2i testPosition) -> bool
		{
			if (_biomeMap.contains(testPosition.x, testPosition.y)
				&& _biomeMap.at(testPosition.x, testPosition.y) == workingType)
			{
				return true;
			}
			return false;
		};
		const auto zonePositions = algo::floodFill(position, biomeTypeChecker);
		BiomeZone zone(workingType);
		zone.setID(zoneID);
		for (auto&& pos : zonePositions)
		{
			possibleSpaces.erase(pos);
			zone.put(pos);
		}
		_zones.emplace(zoneID++, zone);

	}

	std::cout << "Zones identified: " << _zones.size() << std::endl;
}

void drft::gen::WorldGenerator::fillBiomeMap()
{
	for (int y = 0; y < _dimensions.y; ++y)
	{
		for (int x = 0; x < _dimensions.x; ++x)
		{
			if (const Biome* biome = determineBiome({ x*CHUNK_SIZE.x, y*CHUNK_SIZE.y }))
			{
				_biomeMap.at(x, y) = biome;
			}
		}
	}
}

double drft::gen::WorldGenerator::getPerlinAt(const std::string& mapType, sf::Vector2i tilePosition) const
{
	if (!_noiseLayers.contains(mapType))
	{
		throw std::exception(std::string("Map type " + mapType +  " does not exist.").c_str());
	}

	return _noiseLayers.at(mapType).getValueAt(tilePosition);
}

void drft::gen::WorldGenerator::finalizeChunk(sf::Vector2i coordinate, entt::registry& registry) const
{
	const auto& entities = _currentChunkGenerations.at(coordinate).entities;
	placeEntities(entities, registry);
	updateCompletedChunks(coordinate);
}

sf::Vector2i drft::gen::WorldGenerator::getStartingPosition(const std::string& biomeType) const
{
	unsigned int largestForestID = 0;
	int largestSize = 0;

	const auto& biome = _biomeRegistry.get(biomeType);
	for (auto& [id, zone] : _zones)
	{
		if (zone.getType() == &biome)
		{
			if (zone.size() > largestSize)
			{
				largestSize = zone.size();
				largestForestID = id;
			}
		}
	}

	sf::Vector2i coordinate;
	if (!_zones.contains(largestForestID))
	{
		int rand_x = rng::RandomNumberGenerator::intInRange(0, _dimensions.x);
		int rand_y = rng::RandomNumberGenerator::intInRange(0, _dimensions.y);
		coordinate = { rand_x, rand_y };
	}
	else
	{
		coordinate = *_zones.at(largestForestID).getZone().begin();
	}
	
	return spatial::toTileSpace(coordinate);
}

sf::Vector2i drft::gen::WorldGenerator::getDimensions() const
{
	return _dimensions;
}

void drft::gen::WorldGenerator::fixedUpdate(const entt::registry& registry)
{
	auto camera = system::getCurrentCamera(registry);

	float altitude = getRangeFromPerlin("Altitude", _noiseLayers.at("Altitude").getValueAt(camera.position));
	service::DebugInfo::instance().putInfo("Altitude", std::to_string(altitude));

	float humidity = getRangeFromPerlin("Humidity", _noiseLayers.at("Humidity").getValueAt(camera.position));
	service::DebugInfo::instance().putInfo("Humidity", std::to_string(humidity));

	float temperature = getRangeFromPerlin("Temperature", _noiseLayers.at("Temperature").getValueAt(camera.position));
	service::DebugInfo::instance().putInfo("Temperature", std::to_string(temperature));

	const auto* biome = determineBiome(camera.position);
	service::DebugInfo::instance().putInfo("Biome", biome->getName());
}

float drft::gen::WorldGenerator::getRangeFromPerlin(const std::string& mapName, double perlinValue) const
{
	if (!_globalRanges.contains(mapName))
	{
		throw std::exception(std::string("Noise map name" + mapName + " does not exist.").c_str());
	}
	double result = math::remap(0.0, 1.0, _globalRanges.at(mapName).getMin(), _globalRanges.at(mapName).getMax(), perlinValue);
	return static_cast<float>(result);
}

const Biome* drft::gen::WorldGenerator::determineBiome(sf::Vector2i tilePosition) const
{
	if (_biomeCache.contains(tilePosition))
	{
		return _biomeCache.at(tilePosition);
	}

	std::unordered_map<const Biome*, float> biomeScore;
	std::unordered_map<std::string, float> values;

	for (auto& [rangeName, range] : _globalRanges)
	{
		double perlin = getPerlinAt(rangeName, tilePosition);
		float val = getRangeFromPerlin(rangeName, perlin);
		values.emplace(rangeName, val);
	}

	_biomeRegistry.forEachBiome(
		[&](const auto, const Biome& biome)
		{
			float total = 0.0f;
			for (auto& [rangeName, value] : values)
			{
				const auto& globalRange = _globalRanges.at(rangeName);
				float deviation = biome.getDeviationFromClimate(rangeName, value);
				const float maxDeviation = std::max(std::abs(globalRange.getMax() - value), std::abs(value - globalRange.getMin()));
				const float normalizedDeviation = deviation / maxDeviation;
				total += normalizedDeviation;
			}
			biomeScore.emplace(&biome, total);
		});

	const Biome* result;
	float lowest = FLT_MAX;
	for (auto&& [biome, score] : biomeScore)
	{
		if (score < lowest)
		{
			result = biome;
			lowest = score;
		}
	}

	_biomeCache.emplace(tilePosition, result);
	return result;
}

void drft::gen::WorldGenerator::placeStructures(GenerationContext& context, const Biome* biome) const
{
	for (auto&& [name, probability] : biome->getStructureProbabilities())
	{
		if (!rng::percentChance(probability * 100.0)) continue;

		if (auto structure = _structureFactory.build(name))
		{
			// TODO: Find spot that fits structure... (anywhere with no liquid?) - Random for now
			const int randx = rng::RandomNumberGenerator::intInRange(context.area.left, context.area.left + context.area.width);
			const int randy = rng::RandomNumberGenerator::intInRange(context.area.top, context.area.top + context.area.height);

			structure->stamp({ randx, randy }, context);
		}
	}
}

void drft::gen::WorldGenerator::placeTile(sf::Vector2i position, GenerationContext& context) const
{
	context.entityPositions["Tile"].emplace(position);
}

void drft::gen::WorldGenerator::placeLiquid(sf::Vector2i position, GenerationContext& context) const
{
	const double val = _noiseLayers.at("Altitude").getValueAt(position);
	const float height = getRangeFromPerlin("Altitude", val);
	if (height > 0.f) return;

	context.grid.at(position.x, position.y).insert("liquid"_hs);
	context.entityPositions["Water"].emplace(position);
}

void drft::gen::WorldGenerator::generateEntities(sf::Vector2i position, int pass, GenerationContext& context, const Biome* biome) const
{
	if (context.grid.at(position.x, position.y).contains("liquid"_hs)) return;
	if (context.grid.at(position.x, position.y).contains("structure"_hs)) return;

	const auto& entitySlots = biome->getEntitySlots();
	const auto& entityPacks = biome->getEntityPacks();

	for (auto&& [slotName, slot] : entitySlots)
	{
		if (!entityPacks.contains(slotName)) continue;

		auto choice = rng::weightedSelection(entityPacks.at(slotName));
		if (choice < 0) continue;
		const auto& [entityName, _] = entityPacks.at(slotName)[choice];

		float probability = slot.probability;
		if (pass > 0)
		{
			// Only apply multipliers after base probabilty has been set
			for (auto&& multiplier : slot.multipliers)
			{
				probability *= multiplier->apply(position, context);
			}
		}

		if (rng::percentChance(probability * 100.0))
		{
			context.entityPositions[entityName].emplace(position);
			context.grid.at(position.x, position.y).insert(slotName);
		}
		else
		{
			context.entityPositions[entityName].erase(position);
			context.grid.at(position.x, position.y).erase(slotName);
		}
	}
}

void drft::gen::WorldGenerator::placeEntities(const EntityPositionMap& entities, entt::registry& registry) const
{
	const auto& factory = registry.ctx().get<EntityFactory&>();
	for (auto&& [name, positions] : entities)
	{
		for (auto&& position : positions)
		{
			placeSingle(name, position, registry, factory);
		}
	}
}

void drft::gen::WorldGenerator::updateCompletedChunks(sf::Vector2i coordinate) const
{
	_currentChunkGenerations.erase(coordinate);

	auto neighbours = spatial::getIntRectAroundOrigin(coordinate, 3, 3);
	for (auto&& neighbour : neighbours)
	{
		_completedChunks[neighbour]++;
		if (_completedChunks.at(neighbour) >= 9) // Chunk is surrounded (includes self)
		{
			// prevents the accumulation of unnecessary tag grids
			_tagGrid->discard(neighbour);
			// prevents accumulation of unnecessary subchunk biome ptrs
			eraseFromBiomeCache(coordinate);
		}
	}
}

sf::IntRect drft::gen::WorldGenerator::determinePlacementArea(sf::Vector2i coordinate) const
{
	sf::Vector2i tileOrigin = spatial::toTileSpace(coordinate);
	sf::IntRect result = { tileOrigin.x, tileOrigin.y, CHUNK_SIZE.x, CHUNK_SIZE.y };
	return result;
}

void drft::gen::WorldGenerator::initializeGlobalRanges()
{
	_biomeRegistry.forEachBiome(
		[this](const std::string& name, const Biome& biome)
		{
			for (const auto& [layerName, layer] : _noiseLayers)
			{
				if (!biome.containsClimateRange(layerName)) continue;

				auto& globalRange = _globalRanges[layerName];
				const auto biomeRange = biome.getClimateRange(layerName);
				if (biomeRange.isInfinite()) continue;

				float min = std::min(globalRange.getMin(), biomeRange.getMin());
				float max = std::max(globalRange.getMax(), biomeRange.getMax());
				globalRange.setMin(min);
				globalRange.setMax(max);
			}
		});
}

void drft::gen::WorldGenerator::eraseFromBiomeCache(sf::Vector2i coordinate) const
{
	_biomeCache.erase(spatial::toTileSpace(coordinate));
}

