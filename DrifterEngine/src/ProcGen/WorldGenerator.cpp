#include "pch.h"
#include "WorldGenerator.h"
#include "Spatial/Conversions.h"
#include "Spatial/Helpers.h"
#include "Spatial/Grid.h"
#include "Random/RandomNumberGenerator.h"
#include "Random/PerlinNoise.h"
#include "Random/RandomNoise.h"
#include "ProcGen/PlacementAlgorithms/GenerationParameters.h"
#include "PlacementAlgorithms/Algorithms.h"

#include "Machines/Chest.h"
#include "Machines/HeartShrine.h"
#include "Machines/WildernessHorde.h"
#include "Machines/OreDeposit.h"

// Perlin noise cutoffs:
//------------------------------------------

// Temperature cutoffs
static constexpr double TEMPERATURE_COLD = 0.43;
static constexpr double TEMPERATURE_MIDPOINT = 0.50;
static constexpr double TEMPERATURE_HOT = 0.57;

// Altitude cutoffs
static constexpr double ALTITUDE_LOW = 0.43;
static constexpr double ALTITUDE_MIDPOINT = 0.50;
static constexpr double ALTITUDE_VERYHIGH = 0.57;

// Moisture cutoffs
static constexpr double MOISTURE_DRY = 0.43;
static constexpr double MOISTURE_MIDPOINT = 0.50;
static constexpr double MOISTURE_VERYHUMID = 0.57;

//--------------------------------------------



drft::gen::WorldGenerator::WorldGenerator()
{
    
}

void drft::gen::WorldGenerator::setSeed(unsigned int seed)
{
    _seed = seed;

    const unsigned int temperatureSeed = rng::noise(_seed);
    const unsigned int altitudeSeed = rng::noise(temperatureSeed);
    const unsigned int moistureSeed = rng::noise(altitudeSeed);

    _temperatureNoise = std::make_unique<rng::PerlinNoise>(temperatureSeed, 8, 2, 0.5f);
    _altitudeNoise = std::make_unique<rng::PerlinNoise>(altitudeSeed, 8, 2, 0.6f);
    _moistureNoise = std::make_unique<rng::PerlinNoise>(moistureSeed, 8, 2, 0.5f);
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
			for (auto& temperatureType : biome.value["Climate"]["Temperature"].GetArray())
			{
				biomeObj.temperatures.insert(TemperatureString2Enum.at(temperatureType.GetString()));
			}
			for (auto& humidityType : biome.value["Climate"]["Humidity"].GetArray())
			{
				biomeObj.humidities.insert(HumidityString2Enum.at(humidityType.GetString()));
			}
			for (auto& altitudeType : biome.value["Climate"]["Altitude"].GetArray())
			{
				biomeObj.altitudes.insert(AltitudeString2Enum.at(altitudeType.GetString()));
			}
		}
		_biomeTypes.push_back(biomeObj);
	}
}

void drft::gen::WorldGenerator::generateTerrain(sf::Vector2i dimensions)
{
	_biomeMap.resize(dimensions.x, dimensions.y);
	double minTemp = 100;
	double maxTemp = 0;
	double minHum = 100;
	double maxHum = 0;
	double minAlt = 100;
	double maxAlt = 0;

	for (int y = 0; y < dimensions.y; ++y)
	{
		for (int x = 0; x < dimensions.x; ++x)
		{
			const double x_normalized = static_cast<double>(x*spatial::CHUNK_WIDTH) / static_cast<double>(dimensions.x/3*spatial::CHUNK_WIDTH);
			const double y_normalized = static_cast<double>(y*spatial::CHUNK_HEIGHT) / static_cast<double>(dimensions.y/2*spatial::CHUNK_HEIGHT);
			auto temperature = _temperatureNoise->gen(x_normalized, y_normalized);
			auto humidity = _moistureNoise->gen(x_normalized, y_normalized);
			auto altitude = _altitudeNoise->gen(x_normalized, y_normalized);
			minTemp = std::min(minTemp, temperature);
			maxTemp = std::max(maxTemp, temperature);
			minHum = std::min(minHum, humidity);
			maxHum = std::max(maxHum, humidity);
			minAlt = std::min(minAlt, altitude);
			maxAlt = std::max(maxAlt, altitude);

			auto potentialBiomes = biomesThatSatisfy(
				convertTemperatureFromPerlin(temperature),
				convertHumidityFromPerlin(humidity),
				convertAltitudeFromPerlin(altitude)
			);
			if (potentialBiomes.empty())
			{

			}
			else if (potentialBiomes.size() == 1)
			{
				_biomeMap.at(x, y) = *potentialBiomes.begin();
			}
			else if (potentialBiomes.size() > 1)
			{
				// if there are multiple potential biomes, place one that is already near by
				auto surroundings = spatial::getIntRectAroundOrigin({ x,y }, 1, 1);
				for (auto position : surroundings)
				{
					if (position.x < 0 || position.y < 0
						|| position.x >= dimensions.x || position.y >= dimensions.y) continue;
					if (potentialBiomes.contains(_biomeMap.at(position.x, position.y)))
					{
						_biomeMap.at(x, y) = _biomeMap.at(position.x, position.y);
						break;
					}
				}
				// if no matches, choose at random
				if (!_biomeMap.at(x, y))
				{
					int choice = rng::RandomNumberGenerator::intInRange(0, potentialBiomes.size()-1);
					auto itr = potentialBiomes.begin();
					std::advance(itr, choice);
					_biomeMap.at(x, y) = *itr;
				}
			}
		}
	}

	std::cout << "Temperature:\t\tMin: " << minTemp << " Max: " << maxTemp << std::endl;
	std::cout << "Humidity:\t\tMin: " << minHum << " Max: " << maxHum << std::endl;
	std::cout << "Altitude:\t\tMin: " << minAlt << " Max: " << maxAlt << std::endl;
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

drft::gen::TemperatureRange drft::gen::WorldGenerator::convertTemperatureFromPerlin(double perlinTemperature) const
{
	if (perlinTemperature < TEMPERATURE_COLD) return TemperatureRange::Cold;
	if (perlinTemperature < TEMPERATURE_MIDPOINT) return TemperatureRange::Cool;
	if (perlinTemperature < TEMPERATURE_HOT) return TemperatureRange::Warm;
	return TemperatureRange::Hot;
}

drft::gen::HumidityRange drft::gen::WorldGenerator::convertHumidityFromPerlin(double perlinHumidity) const
{
	if (perlinHumidity < MOISTURE_DRY) return HumidityRange::Dry;
	if (perlinHumidity < MOISTURE_MIDPOINT) return HumidityRange::Moderate;
	if (perlinHumidity < MOISTURE_VERYHUMID) return HumidityRange::Humid;
	return HumidityRange::VeryHumid;
}

drft::gen::AltitudeRange drft::gen::WorldGenerator::convertAltitudeFromPerlin(double perlinAltitude) const
{
	if (perlinAltitude < ALTITUDE_LOW) return AltitudeRange::Low;
	if (perlinAltitude < ALTITUDE_MIDPOINT) return AltitudeRange::Medium;
	if (perlinAltitude < ALTITUDE_VERYHIGH) return AltitudeRange::High;
	return AltitudeRange::VeryHigh;
}

std::unordered_set<drft::gen::Biome*> drft::gen::WorldGenerator::biomesThatSatisfy(TemperatureRange temperature, HumidityRange humidity, AltitudeRange altitude)
{
	std::unordered_set<Biome*> result;
	for (auto& biome : _biomeTypes)
	{
		if (biome.temperatures.contains(temperature) && biome.humidities.contains(humidity) && biome.altitudes.contains(altitude))
		{
			result.insert(&biome);
		}
	}

	return result;
}
