#include "pch.h"
#include "WorldGenerator.h"
#include "Algorithms/FloodFill.h"
#include "Spatial/Conversions.h"
#include "Spatial/Helpers.h"
#include "Spatial/Grid.h"
#include "Structures/StructureInstance.h"
#include "JSON/JSONHelpers.h"
#include "Random/RandomNumberGenerator.h"
#include "Random/RandomNoise.h"
#include "Random/NoiseLayer.h"
#include "Random/PercentChance.h"
#include "ProcGen/SpawningPredicates/ISpawningPredicate.h"
#include "ProcGen/GridBitFlags.h"
#include "ProcGen/GenerationContext.h"
#include "ProcGen/PlaceEntities.h"
#include "Services/DebugInfo.h"
#include "Systems/Helpers/GetCurrentCamera.h"
#include "Utility/stdHashing.h"
#include "Utility/Math.h"

static const sf::Vector2i FULL_CHUNK = { drft::spatial::CHUNK_WIDTH, drft::spatial::CHUNK_HEIGHT };
static const sf::Vector2i SUB_CHUNK = { FULL_CHUNK.x / 8, FULL_CHUNK.y / 8 };

static const std::filesystem::path STATIC_DATA_PATH = ".\\data\\static\\";
static const std::filesystem::path BIOME_FOLDER_PATH = STATIC_DATA_PATH.string() + "biomes";
static const std::filesystem::path STRUCTURE_FOLDER_PATH = STATIC_DATA_PATH.string() + "structures";

drft::gen::WorldGenerator::WorldGenerator()
{
	_bitGrid = std::make_unique<spatial::AutoGrid<std::bitset<32>>>(FULL_CHUNK.x, FULL_CHUNK.y);
}

void drft::gen::WorldGenerator::init()
{

	_biomeMap.resize(_dimensions.x, _dimensions.y);

	_biomeRegistry.createBiomesFromJSON(BIOME_FOLDER_PATH);
	_structureFactory.createStructureBlueprintsFromJSON(STRUCTURE_FOLDER_PATH);

	initializeGlobalRanges();
}

void drft::gen::WorldGenerator::createWorldMapSettingsFromJSON(const std::string& JSONfilename)
{
	std::filesystem::path worldSettingsFilePath;
	worldSettingsFilePath /= STATIC_DATA_PATH;
	worldSettingsFilePath /= "world_generation.json";
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
			_dimensions.x = worldSettings["Dimensions"].GetArray()[0].GetInt();
			_dimensions.y = worldSettings["Dimensions"].GetArray()[1].GetInt();
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
			auto surroundings = spatial::getIntRectAroundOrigin({ x, y }, 3, 3);
			for (auto&& cell : surroundings)
			{
				if (!_biomeMap.contains(cell.x, cell.y) || cell == sf::Vector2i(x, y)) continue;
				if (_biomeMap.at(cell.x, cell.y) == centerBiome)
				{
					isolated = false;
					break;
				}
			}
			if (isolated)
			{
				int choice = 0;
				do
				{
					choice = rng::RandomNumberGenerator::intInRange(0, surroundings.size() - 1);
				} while (!_biomeMap.contains(surroundings[choice].x, surroundings[choice].y)
					|| surroundings[choice] == sf::Vector2i(x, y));

				_biomeMap.at(x, y) = _biomeMap.at(surroundings[choice].x, surroundings[choice].y);
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
			if (const Biome* biome = determineBiome({ x, y }))
			{
				_biomeMap.at(x, y) = biome;
			}
		}
	}
}

double drft::gen::WorldGenerator::getPerlinAt(const std::string& mapType, sf::Vector2i coordinate) const
{
	if (!_noiseLayers.contains(mapType))
	{
		throw std::exception(std::string("Map type " + mapType +  " does not exist.").c_str());
	}

	const unsigned int SAMPLE_POINTS = 8;
	sf::Vector2i tileSpace = spatial::toTileSpace(coordinate);
	double sum = 0.0;
	for (unsigned int i = 0; i < SAMPLE_POINTS; ++i)
	{
		int x_sample = rng::RandomNumberGenerator::intInRange(tileSpace.x, tileSpace.x + FULL_CHUNK.x);
		int y_sample = rng::RandomNumberGenerator::intInRange(tileSpace.y, tileSpace.y + FULL_CHUNK.y);
		sum += _noiseLayers.at(mapType).getValueAt({x_sample, y_sample});
	}

	// return the average
	return sum / SAMPLE_POINTS;
}

void drft::gen::WorldGenerator::finalizeChunk(sf::Vector2i coordinate, entt::registry& registry) const
{
	if (!_biomeMap.contains(coordinate.x, coordinate.y)) return;

	placeMany("Tile", { spatial::toTileSpace(coordinate), FULL_CHUNK }, registry);

	const auto placementArea = determinePlacementArea(coordinate);
	GenerationContext context = { .area = placementArea, .grid = *_bitGrid, .registry = registry, .noiseLayers = _noiseLayers, .seed = _seed };
	placeLiquids(context);

	// Iterate each tile and place entities
	for (int subchunk_y = placementArea.top; subchunk_y < placementArea.top + placementArea.height; subchunk_y += SUB_CHUNK.y)
	{
		for (int subchunk_x = placementArea.left; subchunk_x < placementArea.left + placementArea.width; subchunk_x += SUB_CHUNK.x)
		{
			const Biome* biome = determineBiome({ subchunk_x, subchunk_y });
			for (int y = subchunk_y; y < subchunk_y + SUB_CHUNK.y; y++)
			{
				for (int x = subchunk_x; x < subchunk_x + SUB_CHUNK.x; x++)
				{
					for (auto&& [category, entities] : biome->getEntitySpawningAlgorithms())
					{
						for (auto&& [name, algo] : entities)
						{
							
						}
					}
				}
			}
		}
	}


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
	sf::Vector2i coordinate = *_zones.at(largestForestID).getZone().begin();

	return spatial::toTileSpace(coordinate);
}

BiomeIcon drft::gen::WorldGenerator::getBiomeIcon(sf::Vector2i coordinate) const
{
	if (const auto* biome = _biomeMap.at(coordinate.x, coordinate.y))
	{
		return biome->getIcon();
	}
	return BiomeIcon{};
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

const Biome* drft::gen::WorldGenerator::determineBiome(sf::Vector2i coordinate) const
{
	std::map<float, const Biome*> ranking;
	std::unordered_map<std::string, float> values;

	for (auto& [rangeName, range] : _globalRanges)
	{
		double perlin = getPerlinAt(rangeName, coordinate);
		float val = getRangeFromPerlin(rangeName, perlin);
		values.emplace(rangeName, val);
	}

	_biomeRegistry.forEachBiome(
		[&](const std::string& name, const Biome& biome)
		{
			std::vector<float> distances;
			for (auto& [rangeName, value] : values)
			{
				if (!biome.containsClimateRange(rangeName))
				{
					distances.push_back(0.0f);
					continue;
				}
				const auto& range = _globalRanges.at(rangeName);

				float val = values.at(rangeName);
				float dist = biome.getClimateRange(rangeName).distance(val);
				float distNormalized = math::remap(0.0f, range.getMax() - range.getMin(), 0.f, 1.f, dist);
				distances.push_back(distNormalized);
			}
			float total = std::accumulate(distances.begin(), distances.end(), 0.0f);
			ranking.emplace(total, &biome);
		});

	return ranking.begin()->second;
}

void drft::gen::WorldGenerator::placeStructures(GenerationContext& context, const Biome* biome) const
{
	for (auto&& [name, probability] : biome->getStructureProbabilities())
	{
		if (!rng::percentChance(probability * 100)) continue;

		if (auto structure = _structureFactory.build(name))
		{
			// TODO: Find spot that fits structure... (anywhere with no liquid?) - Random for now
			const int randx = rng::RandomNumberGenerator::intInRange(context.area.left, context.area.left + context.area.width);
			const int randy = rng::RandomNumberGenerator::intInRange(context.area.top, context.area.top + context.area.height);

			structure->stamp({ randx, randy }, context);
		}
	}
}

void drft::gen::WorldGenerator::placeLiquids(GenerationContext& context) const
{
	std::string altitude = "Altitude";

	auto predicate = [this, &altitude](sf::Vector2i position) -> bool
		{
			double val = _noiseLayers.at(altitude).getValueAt(position);
			float height = getRangeFromPerlin(altitude, val);
			if (height > 0.f) return false;
			_bitGrid->at(position.x, position.y).set(GridBitFlags::Liquid, true);
			return true;
		};
	placeManyConditional("Water", context.area, context.registry, predicate);
}

void drft::gen::WorldGenerator::placeEntities(GenerationContext& context, const Biome* biome) const
{
	
}

void drft::gen::WorldGenerator::updateCompletedChunks(sf::Vector2i coordinate) const
{
	auto neighbours = spatial::getIntRectAroundOrigin(coordinate, 3, 3);
	for (auto&& neighbour : neighbours)
	{
		_completedChunks[neighbour]++;
		if (_completedChunks.at(neighbour) >= 9) // Chunk is surrounded (includes self)
		{
			// prevents the accumulation of unnecessary bit grids
			_bitGrid->discard(neighbour);
		}
	}
}

sf::IntRect drft::gen::WorldGenerator::determinePlacementArea(sf::Vector2i coordinate) const
{
	sf::Vector2i tileOrigin = spatial::toTileSpace(coordinate);
	sf::IntRect result = { tileOrigin.x, tileOrigin.y, FULL_CHUNK.x, FULL_CHUNK.y };
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

