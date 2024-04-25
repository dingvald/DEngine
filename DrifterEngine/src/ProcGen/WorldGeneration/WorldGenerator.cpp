#include "pch.h"
#include "WorldGenerator.h"
#include "Algorithms/FloodFill.h"
#include "Spatial/Conversions.h"
#include "Spatial/Helpers.h"
#include "Spatial/Grid.h"
#include "Random/RandomNumberGenerator.h"
#include "Random/PerlinNoise.h"
#include "Random/RandomNoise.h"
#include "Random/NoiseMap.h"
#include "Random/PercentChance.h"
#include "ProcGen/PlacementAlgorithms/Algorithms.h"
#include "ProcGen/GridBitFlags.h"
#include "ShapingFunctions.h"
#include "Structures/StructureBase.h"
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

void drft::gen::WorldGenerator::init(sf::Vector2i dimensions, unsigned int seed)
{
	_dimensions = dimensions;
	_biomeMap.resize(_dimensions.x, _dimensions.y);
    _seed = seed;

	for (auto& [_, noiseMap] : _noiseMaps)
	{
		noiseMap.resize(dimensions.x, dimensions.y);
	}
	
	// Load all structure files
	std::cout << "Loading structures..." << std::endl;
	const std::string structuresDirectory = STATIC_DATA_PATH + "structures";
	for (const auto& fileName : std::filesystem::directory_iterator(structuresDirectory))
	{
		std::cout << "Loading " << fileName.path().filename() << std::endl;
		_structureRegistry.loadStructures(fileName.path().filename().string());
	}
	_structureRegistry.resolveAllSubstructures();
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
				if (!_noiseMaps.contains(name))
				{
					_noiseMaps.emplace(name, NoiseMap());
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
						BiomeType::SpawningAlgorithm spawningAlgorithm;
						spawningAlgorithm.name = algoName;
						for (auto& param : entity.value["Parameters"].GetObject())
						{
							spawningAlgorithm.parameters[param.name.GetString()] = param.value.GetFloat();
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
	// Generate starting noise maps
	unsigned int currentSeed = _seed;
	for (auto& [name, noiseMap] : _noiseMaps)
	{
		noiseMap = rng::NoiseMap::generate(_dimensions, { 1,1 }, currentSeed);
		currentSeed = rng::noise(currentSeed);
	}
	_noiseMaps.at("Volcanism") = rng::NoiseMap::generate(_dimensions, { 1,1 }, currentSeed, 16, 2.0, 0.55);

	// Set north pole
	customShaper(_noiseMaps.at("Temperature"), [](double& val, sf::Vector2i position)
		{
			double mod = std::clamp(0.02 * position.y, 0.0, 1.1);
			val = std::clamp(val * mod, 0.0, 1.0);
		});
	// Make volcanism rarer
	customShaper(_noiseMaps.at("Volcanism"), [](double& val, sf::Vector2i position)
		{
			val = std::clamp(val - 0.97, 0.0, 1.0);
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
			for (auto cell : surroundings)
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
	if (!_noiseMaps.contains(mapType))
	{
		throw std::exception(std::string("Map type " + mapType +  " does not exist.").c_str());
	}
	if (!_noiseMaps.at(mapType).contains(coordinate.x, coordinate.y))
	{
		throw std::exception(std::string("Map type " + mapType + " does not contain point " + std::to_string(coordinate.x) + ", " + std::to_string(coordinate.y)).c_str());
	}

	return _noiseMaps.at(mapType).at(coordinate.x, coordinate.y);
}

void drft::gen::WorldGenerator::finalizeChunk(sf::Vector2i coordinate, entt::registry& registry) const
{
    gen::fastFill("Tile", spatial::toTileSpace(coordinate), registry);

	const auto biomeType = _biomeMap.at(coordinate.x, coordinate.y);
	const auto placementArea = determinePlacementArea(coordinate);

	blendBiomeBoundaries(placementArea, coordinate);
	placeStructures(placementArea, biomeType, registry);
	placeEntities(placementArea, biomeType, registry);
	updateCompletedChunks(coordinate);
}

sf::Vector2i drft::gen::WorldGenerator::getStartingPosition(std::string biomeType) const
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

float drft::gen::WorldGenerator::getRangeFromPerlin(const std::string& mapName, double perlinValue) const
{
	if (!_ranges.contains(mapName))
	{
		throw std::exception(std::string("Noise map name" + mapName + " does not exist.").c_str());
	}

	return math::remap(0.0, 1.0, _ranges.at(mapName).getMin(), _ranges.at(mapName).getMax(), perlinValue);
}

std::unordered_set<std::string> drft::gen::WorldGenerator::determinePotentialBiomes(sf::Vector2i coordinate) const
{
	constexpr float ACCEPTANCE_DISTANCE = 0.02; // percent "closeness" to best match
	std::unordered_set<std::string> result;
	std::vector<std::pair<const BiomeType*, float>> distanceMap;
	
	for (auto& [name, biome] : _biomeTypes)
	{
		std::vector<float> distances;
		for (auto& [rangeName, range] : _ranges)
		{
			double perlin = _noiseMaps.at(rangeName).at(coordinate.x, coordinate.y);
			float val = getRangeFromPerlin(rangeName, perlin);
			if (biome.ranges.contains(rangeName))
			{
				float dist = biome.ranges.at(rangeName).distance(val);
				dist = math::remap(0.0, range.getMax() - range.getMin() - val, 0.0, 1.0, dist);
				distances.push_back(dist);
			}
			else
			{
				distances.push_back(1.0);
			}
		}
		float total = std::accumulate(distances.begin(), distances.end(), 0.0f);
		// if perfect match found
		if (total < FLT_EPSILON)
		{
			result.insert(name);
		}
		else
		{
			distanceMap.push_back(std::make_pair(&biome, total / distances.size()));
		}
	}
	

	// If no perfect matches, find closest
	if (result.empty())
	{
		std::stable_sort(distanceMap.begin(), distanceMap.end(), [](const std::pair<const BiomeType*, float>& a, const std::pair<const BiomeType*, float>& b)
			{
				return a.second < b.second;
			});

		auto& [bestMatch, closestDistance] = distanceMap.at(0);
		result.insert(bestMatch->name);
		for (int i = 1; i < distanceMap.size(); ++i)
		{
			auto& [otherMatch, distance] = distanceMap.at(i);
			if (std::abs(closestDistance - distance) >= ACCEPTANCE_DISTANCE)
			{
				break;
			}
			result.insert(otherMatch->name);
		}
	}
	
	return result;
}

const drft::gen::BiomeType* drft::gen::WorldGenerator::selectBiomeType(sf::Vector2i coordinate) const
{
	const auto potentialBiomes = determinePotentialBiomes(coordinate);

	if (potentialBiomes.empty())
	{
		throw std::exception("No biome type found for coordinate");
		return nullptr;
	}
	else if (potentialBiomes.size() == 1)
	{
		return &_biomeTypes.at(*potentialBiomes.begin());
	}
	else if (potentialBiomes.size() > 1)
	{
		// if there are multiple potential biomes, high chance to continue placing the same type that is nearby
		constexpr int CHANCE = 90;

		if (rng::percentChance(CHANCE))
		{
			// Place same biome type
			auto surroundings = spatial::getIntRectAroundOrigin({ coordinate.x, coordinate.y }, 3, 3);
			// Shuffle to prevent bias towards top-left
			std::shuffle(surroundings.begin(), surroundings.end(), rng::RandomNumberGenerator::getGenerator());
			for (auto position : surroundings)
			{
				if (position.x < 0 || position.y < 0
					|| position.x >= _dimensions.x || position.y >= _dimensions.y) continue;
				if (_biomeMap.at(position.x, position.y) && potentialBiomes.contains(_biomeMap.at(position.x, position.y)->name))
				{
					return _biomeMap.at(position.x, position.y);
				}
			}
		}

		// if no matches, choose at random
		if (!_biomeMap.at(coordinate.x, coordinate.y))
		{
			int choice = rng::RandomNumberGenerator::intInRange(0, potentialBiomes.size() - 1);
			auto itr = potentialBiomes.begin();
			std::advance(itr, choice);
			return &_biomeTypes.at(*itr);
		}
	}

	return nullptr;
}

void drft::gen::WorldGenerator::blendBiomeBoundaries(sf::IntRect area, sf::Vector2i coordinate) const
{
	const auto biome = _biomeMap.at(coordinate.x, coordinate.y);
	auto neighbours = spatial::getAdjacentPoints(coordinate, AdjacentType::Cardinal);
	bool onBoundary = false;
	std::vector<sf::Vector2i> diffs;
	diffs.reserve(neighbours.size());
	for (auto neighbour : neighbours)
	{
		if (!_biomeMap.contains(neighbour.x, neighbour.y)) continue;
		if (biome == _biomeMap.at(neighbour.x, neighbour.y)) continue;
		onBoundary = true;
		diffs.push_back(neighbour - coordinate);
	}
	if (!onBoundary) return;

	auto noiseMap = rng::NoiseMap::generate({ area.width, area.height }, { 8,8 }, coordinate.x + coordinate.y + _seed);
	sf::Vector2i centerPoint = { area.width / 2, area.height / 2 };
	for (auto diff : diffs)
	{
		sf::IntRect noisyRect;
		noisyRect.width = diff.x != 0 ? 8 : area.width;
		noisyRect.height = diff.y != 0 ? 8 : area.height;
		noisyRect.left = 0;
		noisyRect.top = 0;

		if (diff.x == 1) noisyRect.left += area.width - noisyRect.width;
		if (diff.y == 1) noisyRect.top += area.height - noisyRect.height;

		for (int y = noisyRect.top; y < noisyRect.top + noisyRect.height; ++y)
		{
			for (int x = noisyRect.left; x < noisyRect.left + noisyRect.width; ++x)
			{
				sf::Vector2i testPoint = diff.x == 0 ? sf::Vector2i{centerPoint.x, y} : sf::Vector2i{x, centerPoint.y };
				auto distance = spatial::distance(centerPoint, testPoint);
				float normalizedDistance = (distance / centerPoint.x);
				if ( ((noiseMap.at(x, y) + normalizedDistance) / 2.f) > 0.65)
				{
					_bitGrid->at(area.left + x, area.top + y).set(gen::Reserved);
				}
			}
		}
	}
}

void drft::gen::WorldGenerator::placeStructures(sf::IntRect area, const BiomeType* biomeType, entt::registry& registry) const
{
	for (auto& [name, probability] : biomeType->structures)
	{
		if (rng::RandomNumberGenerator::realInRange(0.0, 1.0) > probability) continue;

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

void drft::gen::WorldGenerator::placeEntities(sf::IntRect area, const BiomeType* biomeType, entt::registry& registry) const
{
	for (auto& [category, entities] : biomeType->entityCategories)
	{
		for (auto& [entityName, algorithm] : entities)
		{
			auto positions = String2Algorithm.at(algorithm.name)(area, *_bitGrid, algorithm.parameters, _seed);
			place(entityName, {area.left, area.top}, positions, registry);
		}
	}
}

void drft::gen::WorldGenerator::updateCompletedChunks(sf::Vector2i coordinate) const
{
	auto neighbours = spatial::getIntRectAroundOrigin(coordinate, 3, 3);
	for (auto neighbour : neighbours)
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
	auto neighbours = spatial::getAdjacentPoints(coordinate, AdjacentType::Cardinal);
	auto diffs = spatial::getPointDeltas(coordinate, neighbours);
	sf::Vector2i tileOrigin = spatial::toTileSpace(coordinate);
	sf::IntRect result = { tileOrigin.x, tileOrigin.y, FULL_CHUNK.x, FULL_CHUNK.y };

	for (auto diff : diffs)
	{
		if (!_biomeMap.contains(coordinate.x + diff.x, coordinate.y + diff.y)) continue;
		
		// Lower biomes overlap higher ones
		const double myHeight = getPerlinAt("Altitude", coordinate);
		const double otherHeight = getPerlinAt("Altitude", coordinate + diff);
		if (myHeight > otherHeight) continue;

		if (diff.x < 0)
		{
			result.left -= QUARTER_CHUNK.x;
			result.width += QUARTER_CHUNK.x;
		}
		else if (diff.x > 0)
		{
			result.width += QUARTER_CHUNK.x;
		}

		if (diff.y < 0)
		{
			result.top -= QUARTER_CHUNK.y;
			result.height += QUARTER_CHUNK.y;
		}
		else if (diff.y > 0)
		{
			result.height += QUARTER_CHUNK.y;
		}
	}

	return result;
}

