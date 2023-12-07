#include "pch.h"
#include "WorldGenerator.h"
#include "Spatial/Conversions.h"
#include "Spatial/Helpers.h"
#include "Spatial/Grid.h"
#include "Random/RandomNumberGenerator.h"
#include "Random/PerlinNoise.h"
#include "Random/RandomNoise.h"
#include "Random/NoiseMap.h"
#include "ProcGen/PlacementAlgorithms/Algorithms.h"
#include "ShapingFunctions.h"
#include "Utility/stdHashing.h"
#include "Utility/Math.h"
#include "Utility/stdHashing.h"

#include "Machines/Chest.h"
#include "Machines/HeartShrine.h"
#include "Machines/WildernessHorde.h"
#include "Machines/OreDeposit.h"

static const sf::Vector2i FULL_CHUNK = { drft::spatial::CHUNK_WIDTH, drft::spatial::CHUNK_HEIGHT };
static const sf::Vector2i HALF_CHUNK = { FULL_CHUNK.x / 2, FULL_CHUNK.y / 2 };
static const sf::Vector2i QUARTER_CHUNK = { FULL_CHUNK.x / 4, FULL_CHUNK.y / 4 };

drft::gen::WorldGenerator::WorldGenerator()
{
    
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
}

void drft::gen::WorldGenerator::loadBiomes(const std::string& JSONfilename)
{
	using namespace rapidjson;

	std::string filepath = ".\\data\\static\\biomes\\" + JSONfilename;

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
		noiseMap = rng::NoiseMap::generate(_dimensions, { 4,4 }, currentSeed);
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

	for (int y = 0; y < _dimensions.y; ++y)
	{
		for (int x = 0; x < _dimensions.x; ++x)
		{
			_biomeMap.at(x, y) = selectBiomeType({ x, y });
		}
	}
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
		auto zonePositions = floodFillZone(position, workingType);
		BiomeZone zone(workingType);
		zone.setID(zoneID);
		for (auto pos : zonePositions)
		{
			possibleSpaces.erase(pos);
			zone.put(pos);
		}
		_zones.emplace(zoneID++, zone);
	}

	std::cout << "Zones identified: " << _zones.size() << std::endl;
}

std::unordered_set<sf::Vector2i> drft::gen::WorldGenerator::floodFillZone(sf::Vector2i startingNode, const BiomeType* type)
{
	std::queue<sf::Vector2i> q;
	std::unordered_set<sf::Vector2i> result;
	std::unordered_set<sf::Vector2i> visited;
	q.push(startingNode);

	while (!q.empty())
	{
		sf::Vector2i current = q.front();
		q.pop();
		visited.insert(current);
		if (_biomeMap.contains(current.x, current.y) && _biomeMap.at(current.x, current.y) == type)
		{
			result.insert(current);
			for (int y = -1; y <= 1; ++y)
			{
				for (int x = -1; x <= 1; ++x)
				{
					if (visited.contains(current + sf::Vector2i(x, y))) continue;
					q.push(current + sf::Vector2i(x, y));
					visited.insert(current + sf::Vector2i(x, y));
				}
			}
		}
	}

	return result;
}

double drft::gen::WorldGenerator::getPerlinAt(const std::string& mapType, sf::Vector2i coordinate) const
{
	if (!_noiseMaps.contains(mapType))
	{
		throw std::exception(std::string("Map type does not exist." + mapType).c_str());
	}
	if (!_noiseMaps.at(mapType).contains(coordinate.x, coordinate.y))
	{
		throw std::exception(std::string("Map type does not contain point." + coordinate.x + std::string(", ") + std::to_string(coordinate.y)).c_str());
	}

	return _noiseMaps.at(mapType).at(coordinate.x, coordinate.y);
}

void drft::gen::WorldGenerator::finalizeChunk(sf::Vector2i coordinate, entt::registry& registry) const
{
    // Always place tiles
    gen::fastFill("Tile", spatial::toTileSpace(coordinate), registry);
	//
	
	// blend into adjacent chunks
	sf::Vector2i tileOrigin = spatial::toTileSpace(coordinate) - QUARTER_CHUNK;
	spatial::Grid<std::bitset<32>> bitgrid(FULL_CHUNK.x + HALF_CHUNK.x, FULL_CHUNK.y + HALF_CHUNK.y);

	blendBiomeBoundaries(coordinate, bitgrid);

	// Place entities into available spaces
	auto biomeType = _biomeMap.at(coordinate.x, coordinate.y);
	for (auto& [category, entities] : biomeType->entityCategories)
	{
		for (auto& [entityName, algorithm] : entities)
		{
			auto positions = String2Algorithm.at(algorithm.name)(bitgrid, algorithm.parameters, _seed);
			place(entityName,tileOrigin, positions, registry);
		}
	}
}

sf::Vector2i drft::gen::WorldGenerator::getStartingPosition(std::string biomeType) const
{
	unsigned int largestForestID = 0;
	int largestSize = 0;
	for (auto& [id, zone] : _zones)
	{
		if (zone.getType()->name.compare(biomeType) == 0)
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
			if (std::abs(closestDistance - distance) < ACCEPTANCE_DISTANCE)
			{
				result.insert(otherMatch->name);
			}
			else
			{
				break;
			}
		}
	}
	
	return result;
}

const drft::gen::BiomeType* drft::gen::WorldGenerator::selectBiomeType(sf::Vector2i coordinate) const
{

	auto potentialBiomes = determinePotentialBiomes(coordinate);

	if (potentialBiomes.empty())
	{
		return nullptr;
	}
	else if (potentialBiomes.size() == 1)
	{
		return &_biomeTypes.at(*potentialBiomes.begin());
	}
	else if (potentialBiomes.size() > 1)
	{
		/*
		// if there are multiple potential biomes, high chance to continue placing the same type that is nearby
		*/
		constexpr int CHANCE_MAX = 9;
		int choice = rng::RandomNumberGenerator::intInRange(0, CHANCE_MAX);

		if (choice > 1)
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

void drft::gen::WorldGenerator::blendBiomeBoundaries(sf::Vector2i coordinate, spatial::Grid<std::bitset<32>>& bitgrid) const
{
	bitgrid.fill([q_chunk = QUARTER_CHUNK, f_chunk = FULL_CHUNK](int x, int y) -> std::bitset<32>
		{
			if ((x >= q_chunk.x && x < q_chunk.x + f_chunk.x)
			&& (y >= q_chunk.y && y < q_chunk.y + f_chunk.y))
			{
				return std::bitset<32>().reset(0);
			}
			return std::bitset<32>().set(0);
		});
	NoiseMap noiseMap = rng::NoiseMap::generate({ bitgrid.width(), bitgrid.height() }, { 3,3 }, _seed + coordinate.x * coordinate.y, 8, 3.0f, 0.55f);
	sf::Vector2i center = { bitgrid.width() / 2, bitgrid.height() / 2 };

	std::vector<sf::Vector2i> differentSurroundings;
	auto surroundings = spatial::getIntRectAroundOrigin(coordinate, 3, 3);
	for (auto& surrounding : surroundings)
	{
		if (!_biomeMap.contains(surrounding.x, surrounding.y)) continue;
		auto myBiome = _biomeMap.at(coordinate.x, coordinate.y);
		auto otherBiome = _biomeMap.at(surrounding.x, surrounding.y);
		if (myBiome != otherBiome)
		{
			differentSurroundings.push_back(surrounding);
		}
	}
	for (auto& diff : differentSurroundings)
	{
		sf::Vector2i delta = diff - coordinate;
		int width = delta.x == 0 ? FULL_CHUNK.x : HALF_CHUNK.x;
		int height = delta.y == 0 ? FULL_CHUNK.y : HALF_CHUNK.y;
		int x_origin = delta.x < 0 ? 0 : (delta.x == 0 ? QUARTER_CHUNK.x : FULL_CHUNK.x);
		int y_origin = delta.y < 0 ? 0 : (delta.y == 0 ? QUARTER_CHUNK.y : FULL_CHUNK.y);
		float radius = QUARTER_CHUNK.x + HALF_CHUNK.x;
		if (delta.x != 0 && delta.y != 0)
		{
			radius += QUARTER_CHUNK.x / 2;
		}

		for (int y = y_origin; y < y_origin + height; ++y)
		{
			for (int x = x_origin; x < x_origin + width; ++x)
			{
				if (x < 0 || x > noiseMap.width()) continue;
				if (y < 0 || y > noiseMap.height()) continue;
				int x_center = delta.y == 0 ? center.x : (delta.x != 0 ? center.x : x);
				int y_center = delta.x == 0 ? center.y : (delta.y != 0 ? center.y : y);
				int x_edge = delta.x == 0 ? x : (delta.x > 0 ? FULL_CHUNK.x + HALF_CHUNK.x : 0);
				int y_edge = delta.y == 0 ? y : (delta.y > 0 ? FULL_CHUNK.y + HALF_CHUNK.y : 0);

				float distance = std::min(radius, spatial::distance(sf::Vector2i(x_center, y_center), { x, y }));

				if (noiseMap.at(x, y) > 0.9 * std::powf(distance / radius, 4.f))
				{
					bitgrid.at(x, y).reset(0);
				}
			}
		}
	}
}
