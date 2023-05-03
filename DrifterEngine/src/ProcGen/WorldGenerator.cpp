#include "pch.h"
#include "WorldGenerator.h"
#include "Biome.h"
#include "BiomeTypes.h"
#include "Spatial/Conversions.h"
#include "Random/PerlinNoise.h"
#include "Random/RandomNoise.h"
#include "ProcGen/PlacementAlgorithms/GenerationParameters.h"
#include "PlacementAlgorithms/Algorithms.h"

// Temperature cutoffs
static constexpr double TEMPERATURE_COLD = 0.4;
static constexpr double TEMPERATURE_HOT = 0.6;

// Altitude cutoffs
static constexpr double ALTITUDE_LOW = 0.4;
static constexpr double ALTITUDE_HIGH = 0.6;

// Moisture cutoffs
static constexpr double MOISTURE_ARID = 0.4;
static constexpr double MOISTURE_HUMID = 0.6;

drft::gen::WorldGenerator::WorldGenerator(unsigned int seed)
	: _seed(seed)
{
	unsigned int temperatureSeed = rng::noise(seed);
	unsigned int altitudeSeed = rng::noise(temperatureSeed);
	unsigned int moistureSeed = rng::noise(altitudeSeed);

	_temperatureNoise = std::make_unique<rng::PerlinNoise>(temperatureSeed);
	_altitudeNoise = std::make_unique<rng::PerlinNoise>(altitudeSeed);
	_moistureNoise = std::make_unique<rng::PerlinNoise>(moistureSeed);
}

drft::gen::BiomeType drft::gen::WorldGenerator::determineBiomeType(double temperature, double altitude, double moisture) const
{
    if (altitude > ALTITUDE_HIGH)
    {
        return BiomeType::Mountain;
    }
    else if (altitude > ALTITUDE_LOW)
    {
        if (moisture > MOISTURE_HUMID)
        {
            if (temperature > TEMPERATURE_HOT)
            {
                return BiomeType::Jungle;
            }
            else if (temperature > TEMPERATURE_COLD)
            {
                return BiomeType::Lake;
            }
            else  if (temperature <= TEMPERATURE_COLD)
            {
                return BiomeType::Lake;
            }
        }
        else if (moisture > MOISTURE_ARID)
        {
            if (temperature > TEMPERATURE_HOT)
            {
                return BiomeType::Jungle;
            }
            else if (temperature > TEMPERATURE_COLD)
            {
                return BiomeType::Forest;
            }
            else  if (temperature <= TEMPERATURE_COLD)
            {
                return BiomeType::Forest;
            }
        }
        else if (moisture <= MOISTURE_ARID)
        {
            return BiomeType::Desert;
        }
    }
    else if (altitude <= ALTITUDE_LOW)
    {
        if (moisture > MOISTURE_HUMID)
        {
            if (temperature > TEMPERATURE_HOT)
            {
                return BiomeType::Swamp;
            }
            else if (temperature > TEMPERATURE_COLD)
            {
                return BiomeType::Lake;
            }
            else if (temperature <= TEMPERATURE_COLD)
            {
                return BiomeType::Lake;
            }
        }
        else if (moisture > MOISTURE_ARID)
        {
            return BiomeType::Grassland;
        }
        else if (moisture <= MOISTURE_ARID)
        {
            return BiomeType::Desert;
        }
    }

    return BiomeType::Forest;
}

sf::Vector2<double> drft::gen::WorldGenerator::convertIntergerCoordinates(sf::Vector2i coord) const
{
    sf::Vector2<double> result;
    result.x = (static_cast<double>(coord.x) + 0.5) / (13 * 64);
    result.y = (static_cast<double>(coord.y) + 0.5) / (13 * 64);

    return result;
}

bool drft::gen::WorldGenerator::loadBiomeBlueprints(std::string filename)
{
    using namespace rapidjson;
    std::string filepath = ".\\data\\static\\biomes\\" + filename;

    std::ifstream ifs{ filepath };

    std::cout << "Loading " << filename << "..." << std::endl;

    if (!ifs.is_open())
    {
        std::cerr << "Could not open file " << filepath << std::endl;
        return false;
    }
    IStreamWrapper isw{ ifs };
    Document doc{};
    doc.ParseStream<kParseCommentsFlag>(isw);

    if (!doc.HasMember("Biomes") || doc.HasParseError())
    {
        std::cout << filename << " could not be parsed." << std::endl;
        return false;
    }

    std::cout << "Parsing " << filename << "..." << std::endl;

    for (auto& blueprint : doc["Biomes"].GetObject())
    {
        BiomeType type = gen::String2Biome.at(blueprint.name.GetString());
        for (auto& category : blueprint.value.GetObject())
        {
            for (auto& entity : category.value.GetObject())
            {
                BiomePrototype prototype;
                auto entityObj = entity.value.GetObject();
                prototype.name = entity.name.GetString();
                prototype.algorithm = entityObj["Algorithm"].GetString();
                for (auto& [name, value] : entityObj["Params"].GetObject())
                {
                    prototype.params[name.GetString()] = value.GetFloat();
                }
                _biomes[type].prototypes[category.name.GetString()].push_back(prototype);
            }
        }
    }

    return true;
}

void drft::gen::WorldGenerator::buildChunk(sf::Vector2i coordinate, entt::registry& registry) const
{
    const auto biomeType = getBiomeType(coordinate);

    const auto& biome = _biomes.at(biomeType);

    const auto tileCoord = spatial::toTileSpace(coordinate);

    const int LARGE_PRIME = 7919; // not that large lol
    const int seed = rng::noise((coordinate.x + LARGE_PRIME + coordinate.y));

    for (auto& [category, entityList] : biome.prototypes)
    {
        for (auto& [entity, algorithm, params] : entityList)
        {
           auto positions = gen::String2Algorithm.at(algorithm)( seed,
               { spatial::CHUNK_WIDTH, spatial::CHUNK_HEIGHT }, params);

           gen::place(entity, spatial::toTileSpace(coordinate), positions, registry);
        }
    }
}

drft::gen::BiomeType drft::gen::WorldGenerator::getBiomeType(sf::Vector2i coordinate) const
{
    auto dCoord = convertIntergerCoordinates(spatial::toTileSpace(coordinate));
    double t_noise = _temperatureNoise->gen(dCoord.x, dCoord.y, 0);
    double a_noise = _altitudeNoise->gen(dCoord.x, dCoord.y, 0);
    double m_noise = _moistureNoise->gen(dCoord.x, dCoord.y, 0);

    return determineBiomeType(t_noise, a_noise, m_noise);
}
