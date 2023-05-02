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

drft::gen::BiomePair drft::gen::WorldGenerator::determineBiomeType(double temperature, double altitude, double moisture) const
{
    BiomePair result = { BiomeType::Forest, BiomeType::Forest };

    if (altitude > ALTITUDE_HIGH)
    {
        result.main = BiomeType::Mountain;
        if (moisture > MOISTURE_HUMID)
        {
            result.secondary = BiomeType::Lake;
        }
        else if (moisture > MOISTURE_ARID)
        {
            result.secondary = BiomeType::Forest;
        }
        else
        {
            result.secondary = BiomeType::Desert;
        }
    }
    else if (altitude > ALTITUDE_LOW)
    {
        if (moisture > MOISTURE_HUMID)
        {
            result.main = BiomeType::Lake;
            if (temperature > TEMPERATURE_HOT)
            {
                result.main = BiomeType::Swamp;
            }
            else
            {
                result.secondary = BiomeType::Swamp;
            }
        }
        else if (moisture > MOISTURE_ARID)
        {
            result.main = BiomeType::Forest;
        }
        else
        {
            result.main = BiomeType::Desert;
            result.secondary = BiomeType::Desert;
        }
    }
    else
    {
        if (moisture > MOISTURE_HUMID)
        {
            result.main = BiomeType::Swamp;
            if (temperature > TEMPERATURE_HOT)
            {
                result.main = BiomeType::Swamp;
            }
            else
            {
                result.secondary = BiomeType::Swamp;
            }
        }
        else if (moisture > MOISTURE_ARID)
        {
            result.main = BiomeType::Grassland;
        }
        else
        {
            result.main = BiomeType::Desert;
            result.secondary = BiomeType::Desert;
        }
    }

    return result;
}

sf::Vector2<double> drft::gen::WorldGenerator::convertIntergerCoordinates(sf::Vector2i coord) const
{
    sf::Vector2<double> result;
    result.x = (static_cast<double>(coord.x) + 0.5) / 13;
    result.y = (static_cast<double>(coord.y) + 0.5) / 13;

    return result;
}

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
                EntityTriplet triplet;
                auto numAlgo = entity.value.GetArray();
                triplet.name = entity.name.GetString();
                triplet.number = numAlgo[0].GetInt();
                triplet.algorithm = numAlgo[1].GetString();

                _biomes[type].prototypes[category.name.GetString()].push_back(triplet);
            }
        }
    }

    return true;
}

void drft::gen::WorldGenerator::buildChunk(sf::Vector2i coordinate, entt::registry& registry) const
{
    const auto biomeType = getBiomeType(coordinate);

    const auto& mainBiome = _biomes.at(biomeType.main);
    const auto& secondaryBiome = _biomes.at(biomeType.secondary);

    const auto tileCoord = spatial::toTileSpace(coordinate);

    gen::GenerationParameters params;
    params.params["Seed"] = static_cast<double>(coordinate.x + coordinate.y) / coordinate.y;

    for (auto& [category, entityList] : mainBiome.prototypes)
    {
        for (auto& [entity, number, algorithm] : entityList)
        {
           auto positions = gen::String2Algorithm.at(algorithm)(number,
               { spatial::CHUNK_WIDTH, spatial::CHUNK_HEIGHT }, params);

           gen::place(entity, spatial::toTileSpace(coordinate), positions, registry);
        }
    }
    for (auto& [category, entityList] : secondaryBiome.prototypes)
    {
        for (auto& [entity, number, algorithm] : entityList)
        {
            auto positions = gen::String2Algorithm.at(algorithm)(number,
                { spatial::CHUNK_WIDTH, spatial::CHUNK_HEIGHT }, params);

            gen::place(entity, spatial::toTileSpace(coordinate), positions, registry);
        }
    }
}

drft::gen::BiomePair drft::gen::WorldGenerator::getBiomeType(sf::Vector2i coordinate) const
{
    auto dCoord = convertIntergerCoordinates(coordinate);
    double t_noise = _temperatureNoise->gen(dCoord.x, dCoord.y, 0);
    double a_noise = _altitudeNoise->gen(dCoord.x, dCoord.y, 0);
    double m_noise = _moistureNoise->gen(dCoord.x, dCoord.y, 0);

    return determineBiomeType(t_noise, a_noise, m_noise);
}
