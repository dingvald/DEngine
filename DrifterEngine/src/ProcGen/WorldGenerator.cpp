#include "pch.h"
#include "WorldGenerator.h"
#include "Biome.h"
#include "BiomeTypes.h"
#include "Spatial/Conversions.h"
#include "Random/PerlinNoise.h"
#include "Random/RandomNoise.h"
#include "Utility/BuildMany.h"

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
            for (auto& entityPair : category.value.GetObject())
            {
                std::pair<std::string, int> pair;
                pair.first = entityPair.name.GetString();
                pair.second = entityPair.value.GetInt();

                _biomes[type].prototypes[category.name.GetString()].push_back(pair);
            }
        }
    }

    return true;
}

void drft::gen::WorldGenerator::buildChunk(sf::Vector2i coordinate, entt::registry& registry) const
{
	double t_noise = _temperatureNoise->gen(coordinate.x, coordinate.y, 0);
	double a_noise = _altitudeNoise->gen(coordinate.x, coordinate.y, 0);
	double m_noise = _moistureNoise->gen(coordinate.x, coordinate.y, 0);
	const auto biomeType = determineBiomeType(t_noise, a_noise, m_noise);

    const auto& mainBiome = _biomes.at(biomeType.main);
    const auto& secondaryBiome = _biomes.at(biomeType.secondary);

    const auto tileCoord = spatial::toTileSpace(coordinate);

    for (auto& [category, entityList] : mainBiome.prototypes)
    {
        for (auto& [entity, number] : entityList)
        {
            // TODO: use custom placement function for each entity
            util::buildMany(entity, number, 
                {tileCoord.x, tileCoord.y, spatial::CHUNK_WIDTH, spatial::CHUNK_HEIGHT }, registry);
        }
    }
    for (auto& [category, entityList] : secondaryBiome.prototypes)
    {
        for (auto& [entity, number] : entityList)
        {
            // TODO: use custom placement function for each entity
            util::buildMany(entity, number / 4,
                { tileCoord.x, tileCoord.y, spatial::CHUNK_WIDTH, spatial::CHUNK_HEIGHT }, registry);
        }
    }
}

drft::gen::BiomePair drft::gen::WorldGenerator::getBiomeType(sf::Vector2i coordinate) const
{
    double t_noise = _temperatureNoise->gen(coordinate.x, coordinate.y, 0);
    double a_noise = _altitudeNoise->gen(coordinate.x, coordinate.y, 0);
    double m_noise = _moistureNoise->gen(coordinate.x, coordinate.y, 0);
    return determineBiomeType(t_noise, a_noise, m_noise);
}
