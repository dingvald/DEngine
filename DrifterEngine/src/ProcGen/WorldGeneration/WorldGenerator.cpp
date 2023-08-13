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

#include "Machines/Chest.h"
#include "Machines/HeartShrine.h"
#include "Machines/WildernessHorde.h"
#include "Machines/OreDeposit.h"

// Perlin noise cutoffs:
//------------------------------------------

// Temperature cutoffs
static constexpr double TEMPERATURE_COLD = 0.25;
static constexpr double TEMPERATURE_MIDPOINT = 0.50;
static constexpr double TEMPERATURE_HOT = 0.75;

// Altitude cutoffs
static constexpr double ALTITUDE_VERYLOW = 0.0;
static constexpr double ALTITUDE_LOW = 0.25;
static constexpr double ALTITUDE_MIDPOINT = 0.50;
static constexpr double ALTITUDE_VERYHIGH = 0.75;

// Moisture cutoffs
static constexpr double MOISTURE_DRY = 0.25;
static constexpr double MOISTURE_MIDPOINT = 0.50;
static constexpr double MOISTURE_VERYHUMID = 0.75;

//--------------------------------------------

static constexpr int NUM_MAXIMA = 7;

drft::gen::WorldGenerator::WorldGenerator()
{
    
}

void drft::gen::WorldGenerator::init(sf::Vector2i dimensions, unsigned int seed)
{
	_dimensions = dimensions;
	_biomeMap.resize(_dimensions.x, _dimensions.y);

    _seed = seed;

	_altitudeMap.resize(_dimensions.x, _dimensions.y);
	_temperatureMap.resize(_dimensions.x, _dimensions.y);
	_humidityMap.resize(_dimensions.x, _dimensions.y);
	
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
		Biome biomeObj;
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
			// Temperature
			auto tempArray = biome.value["Climate"]["Temperature"].GetArray();
			if (tempArray[0].IsString())
			{
				if (std::string(tempArray[0].GetString()).compare("Any") == 0)
				{
					biomeObj.temperature.setMax(FLT_MAX);
					biomeObj.temperature.setMin(-FLT_MAX);
				}
				else
				{
					throw std::invalid_argument("Unexpected parsed value.");
				}
			}
			else
			{
				biomeObj.temperature.setMin(tempArray[0].GetFloat());
				biomeObj.temperature.setMax(tempArray[1].GetFloat());
				_temperatureExtremes.setMin(std::min(tempArray[0].GetFloat(), _temperatureExtremes.getMin()));
				_temperatureExtremes.setMax(std::max(tempArray[1].GetFloat(), _temperatureExtremes.getMax()));
			}
			
			// Humidity
			auto humArray = biome.value["Climate"]["Humidity"].GetArray();
			if (humArray[0].IsString())
			{
				if (std::string(humArray[0].GetString()).compare("Any") == 0)
				{
					biomeObj.humidity.setMax(FLT_MAX);
					biomeObj.humidity.setMin(-FLT_MAX);
				}
				else
				{
					throw std::invalid_argument("Unexpected parsed value.");
				}
			}
			else
			{
				biomeObj.humidity.setMin(humArray[0].GetFloat());
				biomeObj.humidity.setMax(humArray[1].GetFloat());
				_humidityExtremes.setMin(std::min(humArray[0].GetFloat(), _humidityExtremes.getMin()));
				_humidityExtremes.setMax(std::max(humArray[1].GetFloat(), _humidityExtremes.getMax()));
			}

			// Altitude
			auto altArray = biome.value["Climate"]["Altitude"].GetArray();
			if (altArray[0].IsString())
			{
				if (std::string(altArray[0].GetString()).compare("Any") == 0)
				{
					biomeObj.altitude.setMax(FLT_MAX);
					biomeObj.altitude.setMin(-FLT_MAX);
				}
				else
				{
					throw std::invalid_argument("Unexpected parsed value.");
				}
			}
			else
			{
				biomeObj.altitude.setMin(altArray[0].GetFloat());
				biomeObj.altitude.setMax(altArray[1].GetFloat());
				_altitudeExtremes.setMin(std::min(altArray[0].GetFloat(), _altitudeExtremes.getMin()));
				_altitudeExtremes.setMax(std::max(altArray[1].GetFloat(), _altitudeExtremes.getMax()));
			}
		}
		_biomeTypes.push_back(biomeObj);
	}
}

void drft::gen::WorldGenerator::generateTerrain()
{
	// Generate starting noise maps
	_altitudeMap = rng::NoiseMap::generate(_dimensions, { 3,2 }, _seed, 16, 2.0f, 0.55);
	_temperatureMap = rng::NoiseMap::generate(_dimensions, { 3,2 }, rng::noise(_seed));
	_humidityMap = rng::NoiseMap::generate(_dimensions, { 3,2 }, rng::noise(rng::noise(_seed)));

	// Remap altitude map to have negative values for ocean
	remap(0.0, 1.0, -1.0, 1.0, _altitudeMap);

	for (int y = 0; y < _dimensions.y; ++y)
	{
		for (int x = 0; x < _dimensions.x; ++x)
		{
			_biomeMap.at(x, y) = selectBiome({ x, y });
		}
	}

}

void drft::gen::WorldGenerator::finalize(sf::Vector2i coordinate, entt::registry& registry) const
{
    // Always place tiles
    gen::fastFill("Tile", spatial::toTileSpace(coordinate), registry);
}

drft::gen::BiomeIcon drft::gen::WorldGenerator::getBiomeIcon(sf::Vector2i coordinate) const
{
	if (auto biome = _biomeMap.at(coordinate.x, coordinate.y))
	{
		return biome->icon;
	}
	return BiomeIcon{};
}

float drft::gen::WorldGenerator::getTemperatureFromPerlin(double perlinTemperature) const
{
	return math::remap(0.0, 1.0, _temperatureExtremes.getMin(), _temperatureExtremes.getMax(), perlinTemperature);
}

float drft::gen::WorldGenerator::getHumidityFromPerlin(double perlinHumidity) const
{
	return math::remap(0.0, 1.0, _humidityExtremes.getMin(), _humidityExtremes.getMax(), perlinHumidity);
}

float drft::gen::WorldGenerator::getAltitudeFromPerlin(double perlinAltitude) const
{
	return math::remap(-1.0, 1.0, _altitudeExtremes.getMin(), _altitudeExtremes.getMax(), perlinAltitude);
}

std::unordered_set<const drft::gen::Biome*> drft::gen::WorldGenerator::biomesThatSatisfy(float temperature, float humidity, float altitude) const
{
	constexpr float ACCEPTANCE_DISTANCE = 0.2; // 10 percent "closeness"
	std::unordered_set<const Biome*> result;
	std::vector<std::pair<const Biome*, float>> distanceMap;
	for (auto& biome : _biomeTypes)
	{
		float tempDist = biome.temperature.distance(temperature);
		tempDist = math::remap(_temperatureExtremes.getMin(), _temperatureExtremes.getMax(), 0.0, 1.0, tempDist);
		float humDist = biome.humidity.distance(humidity);
		humDist = math::remap(_humidityExtremes.getMin(), _humidityExtremes.getMax(), 0.0, 1.0, humDist);
		float altDist = biome.altitude.distance(altitude);
		altDist = math::remap(_altitudeExtremes.getMin(), _altitudeExtremes.getMax(), 0.0, 1.0, altDist);

		distanceMap.push_back(std::make_pair(&biome,  (tempDist + humDist + altDist) / 3.0f));
	}

	std::sort(distanceMap.begin(), distanceMap.end(), [](const std::pair<const Biome*, float>& a, const std::pair<const Biome*, float>& b)
		{
			return a.second < b.second;
		});

	for (const auto& [biome, distance] : distanceMap)
	{
		if (distance < ACCEPTANCE_DISTANCE)
		{
			result.insert(biome);
		}
	}
	
	return result;
}

const drft::gen::Biome* drft::gen::WorldGenerator::selectBiome(sf::Vector2i coordinate) const
{
	auto temperature = _temperatureMap.at(coordinate.x, coordinate.y);
	auto humidity = _humidityMap.at(coordinate.x, coordinate.y);
	auto altitude = _altitudeMap.at(coordinate.x, coordinate.y);

	auto potentialBiomes = biomesThatSatisfy
	(
		getTemperatureFromPerlin(temperature),
		getHumidityFromPerlin(humidity),
		getAltitudeFromPerlin(altitude)
	);

	if (potentialBiomes.empty())
	{
		return nullptr;
	}
	else if (potentialBiomes.size() == 1)
	{
		return *potentialBiomes.begin();
	}
	else if (potentialBiomes.size() > 1)
	{
		/*
		// if there are multiple potential biomes, high chance to continue placing the same type that is nearby
		*/
		constexpr int CHANCE_MAX = 5;
		constexpr int THRESHOLD = 1;
		int choice = rng::RandomNumberGenerator::intInRange(0, CHANCE_MAX);

		if (choice > THRESHOLD)
		{
			// Place same biome type
			auto surroundings = spatial::getIntRectAroundOrigin({ coordinate.x, coordinate.y }, 3, 3);
			// Shuffle to prevent bias towards top-left
			std::shuffle(surroundings.begin(), surroundings.end(), rng::RandomNumberGenerator::getGenerator());
			for (auto position : surroundings)
			{
				if (position.x < 0 || position.y < 0
					|| position.x >= _dimensions.x || position.y >= _dimensions.y) continue;
				if (potentialBiomes.contains(_biomeMap.at(position.x, position.y)))
				{
					return _biomeMap.at(position.x, position.y);
					break;
				}
			}
		}

		// if no matches, choose at random
		if (!_biomeMap.at(coordinate.x, coordinate.y))
		{
			int choice = rng::RandomNumberGenerator::intInRange(0, potentialBiomes.size() - 1);
			auto itr = potentialBiomes.begin();
			std::advance(itr, choice);
			return *itr;
		}
	}

	return nullptr;
}
