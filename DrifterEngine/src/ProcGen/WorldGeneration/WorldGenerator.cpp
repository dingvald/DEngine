#include "pch.h"
#include "WorldGenerator.h"
#include "Algorithms/FloodFill.h"
#include "Spatial/Conversions.h"
#include "Spatial/Helpers.h"
#include "Spatial/Grid.h"
#include "Random/RandomNumberGenerator.h"
#include "Random/RandomNoise.h"
#include "Random/NoiseLayer.h"
#include "Random/PercentChance.h"
#include "ProcGen/SpawningAlgorithms/FillSpawn.h"
#include "ProcGen/SpawningAlgorithms/NoiseLayerSpawn.h"
#include "ProcGen/SpawningAlgorithms/OrganicSpawn.h"
#include "ProcGen/SpawningAlgorithms/PerlinSpawn.h"
#include "ProcGen/SpawningAlgorithms/RandomSpawn.h"
#include "ProcGen/SpawningAlgorithms/FastFill.h"
#include "ProcGen/GridBitFlags.h"
#include "Structures/StructureBase.h"
#include "Services/DebugInfo.h"
#include "Systems/Helpers/GetCurrentCamera.h"
#include "Utility/stdHashing.h"
#include "Utility/Math.h"

static const sf::Vector2i FULL_CHUNK = { drft::spatial::CHUNK_WIDTH, drft::spatial::CHUNK_HEIGHT };

static const sf::Vector2i HALF_CHUNK = { FULL_CHUNK.x / 2, FULL_CHUNK.y / 2 };
static const sf::Vector2i QUARTER_CHUNK = { FULL_CHUNK.x / 4, FULL_CHUNK.y / 4 };

static const std::string STATIC_DATA_PATH = ".\\data\\static\\";

drft::gen::WorldGenerator::WorldGenerator()
{
	_bitGrid = std::make_unique<spatial::AutoGrid<std::bitset<32>>>(FULL_CHUNK.x, FULL_CHUNK.y);
}

void drft::gen::WorldGenerator::init()
{
	_spawningAlgorithms.registerAlgorithm<FillSpawn>("Fill");
	_spawningAlgorithms.registerAlgorithm<NoiseLayerSpawn>("NoiseLayer");
	_spawningAlgorithms.registerAlgorithm<OrganicSpawn>("Organic");
	_spawningAlgorithms.registerAlgorithm<PerlinSpawn>("Perlin");
	_spawningAlgorithms.registerAlgorithm<RandomSpawn>("Random");

	_biomeMap.resize(_dimensions.x, _dimensions.y);

	// Load all structure files
	std::cout << "Loading structures..." << std::endl;
	const std::string structuresDirectory = STATIC_DATA_PATH + "structures";
	for (const auto& fileName : std::filesystem::directory_iterator(structuresDirectory))
	{
		std::cout << "Loading " << fileName.path().filename() << std::endl;
		_structureRegistry.loadStructures(fileName.path().filename().string());
	}
	_structureRegistry.resolveAllSubstructures();
	std::cout << "Structures loaded." << std::endl;

	loadBiomes("biomes.json");
}

void drft::gen::WorldGenerator::loadWorldMapSettings(const std::string& JSONfilename)
{
	using namespace rapidjson;

	std::string filepath = STATIC_DATA_PATH + JSONfilename;

	std::ifstream ifs{ filepath };

	std::cout << "Loading " << JSONfilename << "..." << std::endl;

	if (!ifs.is_open())
	{
		std::cerr << "Could not open file " << filepath << std::endl;
		return;
	}
	IStreamWrapper isw{ ifs };
	Document doc{};
	doc.ParseStream<kParseCommentsFlag>(isw);

	if (!doc.HasMember("WorldSettings") || doc.HasParseError())
	{
		std::cout << JSONfilename << " could not be parsed." << std::endl;
		return;
	}

	auto& worldSettings = doc["WorldSettings"];

	std::cout << "Parsing " << JSONfilename << "..." << std::endl;

	// TODO: check for saved seed first
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

void drft::gen::WorldGenerator::loadBiomes(const std::string& JSONfilename)
{
	using namespace rapidjson;

	std::string filepath = STATIC_DATA_PATH + "biomes\\" + JSONfilename;

	std::ifstream ifs{ filepath };

	std::cout << "Loading " << JSONfilename << "..." << std::endl;

	if (!ifs.is_open())
	{
		std::cerr << "Could not open file " << filepath << std::endl;
		return;
	}
	IStreamWrapper isw{ ifs };
	Document doc{};
	doc.ParseStream<kParseCommentsFlag>(isw);

	if (!doc.HasMember("Biomes") || doc.HasParseError())
	{
		std::cout << JSONfilename << " could not be parsed." << std::endl;
		return;
	}

	std::cout << "Parsing " << JSONfilename << "..." << std::endl;

	// Iterate each biome
	for (auto&& biome : doc["Biomes"].GetObject())
	{
		BiomeType biomeObj;
		biomeObj.name = biome.name.GetString();
		if (biome.value.HasMember("Icon"))
		{
			biomeObj.icon.sprite = biome.value["Icon"]["Sprite"].GetInt();
			biomeObj.icon.color.r = biome.value["Icon"]["Color"].GetArray()[0].GetInt();
			biomeObj.icon.color.g = biome.value["Icon"]["Color"].GetArray()[1].GetInt();
			biomeObj.icon.color.b = biome.value["Icon"]["Color"].GetArray()[2].GetInt();
		}
		if (biome.value.HasMember("Climate"))
		{
			for (auto& climateRange : biome.value["Climate"].GetObject())
			{
				auto name = climateRange.name.GetString();
				auto range = climateRange.value.GetArray();
				if (!_noiseLayers.contains(name))
				{
					throw std::exception("Noise layer name does not exists. Please refer to world_settings.json for allowed values.");
				}
				if (range[0].IsString())
				{
					if (std::string(range[0].GetString()).compare("Any") == 0)
					{
						biomeObj.ranges[name].setMax(FLT_MAX);
						biomeObj.ranges[name].setMin(-FLT_MAX);
					}
					else
					{
						throw std::invalid_argument("Unexpected parsed value.");
					}
				}
				else
				{
					biomeObj.ranges[name].setMin(range[0].GetFloat());
					biomeObj.ranges[name].setMax(range[1].GetFloat());

					if (_ranges.contains(name))
					{
						_ranges[name].setMin(std::min(range[0].GetFloat(), _ranges[name].getMin()));
						_ranges[name].setMax(std::max(range[1].GetFloat(), _ranges[name].getMax()));
					}
					else
					{
						_ranges[name].setMin(range[0].GetFloat());
						_ranges[name].setMax(range[1].GetFloat());
					}
					
				}
			}
		}
		if (biome.value.HasMember("Entities"))
		{
			for (auto& category : biome.value["Entities"].GetObject())
			{
				for (auto& entity : category.value.GetObject())
				{
					if (entity.value.HasMember("Algorithm")
						&& entity.value.HasMember("Parameters"))
					{
						std::string algoName = entity.value["Algorithm"].GetString();
						if (!_spawningAlgorithms.contains(algoName))
						{
							throw std::exception("Spawning algorithm does not exist");
						}
						BiomeType::SpawningAlgorithm spawningAlgorithm;
						spawningAlgorithm.name = algoName;
						for (auto& param : entity.value["Parameters"].GetObject())
						{
							if (param.value.IsString())
							{
								spawningAlgorithm.parameters[param.name.GetString()] = param.value.GetString();
							}
							else if (param.value.IsBool())
							{
								spawningAlgorithm.parameters[param.name.GetString()] = param.value.GetBool();
							}
							else
							{
								spawningAlgorithm.parameters[param.name.GetString()] = param.value.GetFloat();
							}
						}
						biomeObj.entityCategories
							[category.name.GetString()]
							[entity.name.GetString()] = spawningAlgorithm;
					}
					else
					{
						throw std::exception("Parsed entity does not have a placement algorithm");
					}
				}
			}
		}
		if (biome.value.HasMember("Structures"))
		{
			for (auto& structure : biome.value["Structures"].GetObject())
			{
				biomeObj.structures.emplace_back(structure.name.GetString(), structure.value.GetFloat());
			}
		}
		_biomeTypes.emplace(biomeObj.name, biomeObj);
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
			double mod = std::clamp(0.02 * position.y, 0.0, 1.1);
			return val * mod;
		});
	// Make Volcanism rarer
	_noiseLayers.at("Volcanism").addBiasingFunction(
		[](double val, sf::Vector2i position)
		{
			return val - 0.90;
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
		const BiomeType* workingType = _biomeMap.at(position.x, position.y);
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
			if (auto biomeType = selectBiomeType({ x, y }))
			{
				_biomeMap.at(x, y) = biomeType;
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

    fastFill("Tile", spatial::toTileSpace(coordinate), registry);

	const auto biomeType = _biomeMap.at(coordinate.x, coordinate.y);
	const auto placementArea = determinePlacementArea(coordinate);

	placeLiquids(placementArea, biomeType, registry);
	//placeStructures(placementArea, biomeType, registry);
	placeEntities(placementArea, biomeType, registry);

	updateCompletedChunks(coordinate);
}

sf::Vector2i drft::gen::WorldGenerator::getStartingPosition(const std::string& biomeType) const
{
	unsigned int largestForestID = 0;
	int largestSize = 0;
	for (auto& [id, zone] : _zones)
	{
		if (zone.getType()->name == biomeType)
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

drft::gen::BiomeIcon drft::gen::WorldGenerator::getBiomeIcon(sf::Vector2i coordinate) const
{
	if (auto biome = _biomeMap.at(coordinate.x, coordinate.y))
	{
		return biome->icon;
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
	if (!_ranges.contains(mapName))
	{
		throw std::exception(std::string("Noise map name" + mapName + " does not exist.").c_str());
	}
	double result = math::remap(0.0, 1.0, _ranges.at(mapName).getMin(), _ranges.at(mapName).getMax(), perlinValue);
	return static_cast<float>(result);
}

const drft::gen::BiomeType* drft::gen::WorldGenerator::selectBiomeType(sf::Vector2i coordinate) const
{
	std::vector<std::pair<const BiomeType*, float>> distanceMap;
	std::map<float, const BiomeType*> ranking;

	for (auto& [name, biome] : _biomeTypes)
	{
		std::vector<float> distances;
		for (auto& [rangeName, range] : _ranges)
		{
			double perlin = getPerlinAt(rangeName, coordinate);
			float val = getRangeFromPerlin(rangeName, perlin);

			if (!biome.ranges.contains(rangeName))
			{
				distances.push_back(1.0);
				continue;
			}

			float dist = biome.ranges.at(rangeName).distance(val);
			dist = static_cast<float>(math::remap(0.0, range.getMax() - range.getMin() - val, 0.0, 1.0, dist));
			distances.push_back(dist);
		}
		float total = std::accumulate(distances.begin(), distances.end(), 0.0f);
		ranking.emplace(total, &biome);
	}

	return ranking.begin()->second;
}

void drft::gen::WorldGenerator::placeStructures(sf::IntRect area, const BiomeType* biomeType, entt::registry& registry) const
{
	for (auto& [name, probability] : biomeType->structures)
	{
		if (!rng::percentChance(static_cast<int>(probability*100))) continue;

		const auto& structure = _structureRegistry.lookup(name);
		const auto maxBounds = structure.getMaximumBounds();
		const auto minBounds = structure.getMinimumBounds();

		int rand_x = rng::RandomNumberGenerator::intInRange(0, area.width - maxBounds.x - 1);
		int rand_y = rng::RandomNumberGenerator::intInRange(0, area.height - maxBounds.y - 1);
		int safetyCount = 10;

		while (_bitGrid->at(area.left + rand_x, area.top + rand_y).test(gen::Structure)
			|| _bitGrid->at(area.left + maxBounds.x + rand_x, area.top + maxBounds.y + rand_y).test(gen::Structure)
			&& safetyCount > 0)
		{
			rand_x = rng::RandomNumberGenerator::intInRange(0, area.width - maxBounds.x - 1);
			rand_y = rng::RandomNumberGenerator::intInRange(0, area.height - maxBounds.y - 1);
			--safetyCount;
		}
		if (safetyCount <= 0) continue;

		auto rect = structure.stamp(sf::Vector2i{ area.left + rand_x, area.top + rand_y }, registry);
		_bitGrid->fill({ gen::Structure }, area.left + rand_x, area.top + rand_y, rect.width, rect.height);
	}
}

void drft::gen::WorldGenerator::placeLiquids(sf::IntRect area, const BiomeType* biomeType, entt::registry& registry) const
{
	std::string altitude = "Altitude";
	std::vector<sf::Vector2i> positions;
	for (int y = 0; y < area.height; ++y)
	{
		for (int x = 0; x < area.width; ++x)
		{
			double val = _noiseLayers.at(altitude).getValueAt({ area.left + x, area.top + y });
			float height = getRangeFromPerlin(altitude, val);
			if (height > 0.f) continue;
			positions.push_back({ x, y });
		}
	}

	if (positions.empty()) return;

	place("Water", {area.left, area.top}, positions, registry);
}

void drft::gen::WorldGenerator::placeEntities(sf::IntRect area, const BiomeType* biomeType, entt::registry& registry) const
{
	GenerationContext context = { area, *_bitGrid, _noiseLayers, _seed };
	for (auto& [category, entities] : biomeType->entityCategories)
	{
		for (auto& [entityName, algorithm] : entities)
		{
			auto positions = _spawningAlgorithms.get(algorithm.name).generateSpawnPositions(context, algorithm.parameters);
			place(entityName, {area.left, area.top}, positions, registry);
		}
	}
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
	const auto biomeType = _biomeMap.at(coordinate.x, coordinate.y);
	const double myHeight = getPerlinAt("Altitude", coordinate);

	auto neighbors = spatial::getAdjacentPoints(coordinate, spatial::AdjacentType::Cardinal);
	for (auto&& neighbor : neighbors)
	{
		if (!_biomeMap.contains(neighbor.x, neighbor.y)) continue;

		const auto otherBiomeType = _biomeMap.at(neighbor.x, neighbor.y);
		if (biomeType == otherBiomeType) continue;
		const double otherHeight = getPerlinAt("Altitude", neighbor);
		if (myHeight > otherHeight) continue;

		auto delta = neighbor - coordinate;
		if (delta.x < 0)
		{
			result.left -= QUARTER_CHUNK.x;
			result.width += QUARTER_CHUNK.x;
		}
		else if (delta.x > 0)
		{
			result.width += QUARTER_CHUNK.x;
		}

		if (delta.y < 0)
		{
			result.top -= QUARTER_CHUNK.y;
			result.height += QUARTER_CHUNK.y;
		}
		else if (delta.y > 0)
		{
			result.height += QUARTER_CHUNK.y;
		}
	}

	return result;
}

