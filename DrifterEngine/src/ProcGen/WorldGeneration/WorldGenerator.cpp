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

void drft::gen::WorldGenerator::generateTerrain()
{
	// Generate starting noise maps
	auto altitudeMap = rng::NoiseMap::generate(_dimensions, { 3,2 }, _seed);
	auto temperatureMap = rng::NoiseMap::generate(_dimensions, { 3,2 }, rng::noise(_seed));
	auto humidityMap = rng::NoiseMap::generate(_dimensions, { 3,2 }, rng::noise(rng::noise(_seed)));

	// Place local maxima
	std::unordered_set<sf::Vector2i> maximaPositions;
	for (int maxima = 0; maxima < NUM_MAXIMA; ++maxima)
	{
		float randdrop = rng::RandomNumberGenerator::realInRange(0.03, 0.09);
		sf::Vector2i randpos;
		bool doOver = false;
		int safetyCount = 0;
		do
		{
			randpos.x = rng::RandomNumberGenerator::intInRange(0, _dimensions.x - 1);
			randpos.y = rng::RandomNumberGenerator::intInRange(0, _dimensions.y - 1);
			auto positionsInRadius = spatial::getIntCircleInRadius(randpos, 7);
			for (auto pos : positionsInRadius)
			{
				if (maximaPositions.contains(pos))
				{
					doOver = true;
					break;
				}
			}
			++safetyCount;
		} while (doOver && safetyCount < 10);

		maximaPositions.insert(randpos);
		setDropOffCircle(randpos, 0.75, randdrop, altitudeMap);
	}

	for (int y = 0; y < _dimensions.y; ++y)
	{
		for (int x = 0; x < _dimensions.x; ++x)
		{
			auto temperature = temperatureMap.at(x, y);
			auto humidity = humidityMap.at(x, y);
			auto altitude = altitudeMap.at(x, y) - 0.5;

			auto potentialBiomes = biomesThatSatisfy
			(
				getTemperatureFromPerlin(temperature),
				getHumidityFromPerlin(humidity),
				getAltitudeFromPerlin(altitude)
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
						|| position.x >= _dimensions.x || position.y >= _dimensions.y) continue;
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

drft::gen::TemperatureRange drft::gen::WorldGenerator::getTemperatureFromPerlin(double perlinTemperature) const
{
	if (perlinTemperature < TEMPERATURE_COLD) return TemperatureRange::Cold;
	if (perlinTemperature < TEMPERATURE_MIDPOINT) return TemperatureRange::Cool;
	if (perlinTemperature < TEMPERATURE_HOT) return TemperatureRange::Warm;
	return TemperatureRange::Hot;
}

drft::gen::HumidityRange drft::gen::WorldGenerator::getHumidityFromPerlin(double perlinHumidity) const
{
	if (perlinHumidity < MOISTURE_DRY) return HumidityRange::Dry;
	if (perlinHumidity < MOISTURE_MIDPOINT) return HumidityRange::Moderate;
	if (perlinHumidity < MOISTURE_VERYHUMID) return HumidityRange::Humid;
	return HumidityRange::VeryHumid;
}

drft::gen::AltitudeRange drft::gen::WorldGenerator::getAltitudeFromPerlin(double perlinAltitude) const
{
	if (perlinAltitude < ALTITUDE_VERYLOW) return AltitudeRange::VeryLow;
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
