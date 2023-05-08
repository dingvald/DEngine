#include "pch.h"
#include "WorldGenerator.h"
#include "Biome.h"
#include "BiomeTypes.h"
#include "Spatial/Conversions.h"
#include "Spatial/Helpers.h"
#include "Spatial/Grid.h"
#include "Random/PerlinNoise.h"
#include "Random/RandomNoise.h"
#include "ProcGen/PlacementAlgorithms/GenerationParameters.h"
#include "ProcGen/Helpers/Dilate.h"
#include "PlacementAlgorithms/Algorithms.h"

// Temperature cutoffs
static constexpr double TEMPERATURE_COLD = 0.43;
static constexpr double TEMPERATURE_HOT = 0.57;

// Altitude cutoffs
static constexpr double ALTITUDE_LOW = 0.43;
static constexpr double ALTITUDE_HIGH = 0.57;

// Moisture cutoffs
static constexpr double MOISTURE_ARID = 0.43;
static constexpr double MOISTURE_HUMID = 0.57;

void drft::gen::WorldGenerator::setSeed(unsigned int seed)
{
    _seed = seed;
    _cachedBiomeTypes.clear();

    const unsigned int temperatureSeed = rng::noise(_seed);
    const unsigned int altitudeSeed = rng::noise(temperatureSeed);
    const unsigned int moistureSeed = rng::noise(altitudeSeed);

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
    result.x = (static_cast<double>(coord.x) + 0.5) / (13 * 80);
    result.y = (static_cast<double>(coord.y) + 0.5) / (13 * 80);

    return result;
}

std::vector<sf::Vector2i> drft::gen::WorldGenerator::determineOpenFaces(sf::Vector2i coord) const
{
    std::vector<sf::Vector2i> openFaces;
    const auto myType = getBiomeType(coord);

    // iterate each adjacent chunk
    for (int y = -1; y <= 1; ++y)
    {
        for (int x = -1; x <= 1; ++x)
        {
            // skip self
            if (x == 0 && y == 0) continue;
            const auto otherType = getBiomeType({ coord.x + x, coord.y + y });
            if (myType != otherType)
            {
                openFaces.emplace_back(x, y);
            }
        }
    }

    return openFaces;
}

void drft::gen::WorldGenerator::determineAvailableSpaces(std::vector<sf::Vector2i> openFaces, spatial::Grid<int>& spaces, unsigned int seed) const
{
    const double THRESHOLD = 1.31;
    const double P_WEIGHT = 1.0;
    const double G_WEIGHT = 1.3;
    const int numOpenFaces = openFaces.size();
    rng::PerlinNoise noise(seed,8,3.0);

    const int centerx = spaces.width() / 2;
    const int centery = spaces.height() / 2;

    std::vector<int> xGrad(spaces.width(), centerx);
    std::vector<int> yGrad(spaces.height(), centery);

    for (auto face : openFaces)
    {
        if (std::abs(face.x) != std::abs(face.y))
        {
            if (face.x != 0)
            {
                if (face.x == -1)
                {
                    std::iota(xGrad.begin(), xGrad.begin() + centerx, 1);
                }
                else
                {
                    std::iota(xGrad.rbegin(), xGrad.rbegin() + centerx, 1);
                }
            }
            else
            {
                if (face.y == -1)
                {
                    std::iota(yGrad.begin(), yGrad.begin() + centery, 1);
                }
                else
                {
                    std::iota(yGrad.rbegin(), yGrad.rbegin() + centery, 1);
                }
            }
        }
        else
        {
            auto adjacent1 = std::find(openFaces.begin(), openFaces.end(), sf::Vector2i(face.x, 0));
            auto adjacent2 = std::find(openFaces.begin(), openFaces.end(), sf::Vector2i(0, face.y));

            if (adjacent1 == openFaces.end() && adjacent2 == openFaces.end())
            {
                if (face.x == -1)
                {
                    std::iota(xGrad.begin(), xGrad.begin() + centerx, static_cast<double>(centerx) / 1.8f);
                }
                else
                {
                    std::iota(xGrad.rbegin(), xGrad.rbegin() + centerx, static_cast<double>(centerx) / 1.8f);
                }

                if (face.y == -1)
                {
                    std::iota(yGrad.begin(), yGrad.begin() + centery, static_cast<double>(centerx) / 1.8f);
                }
                else
                {
                    std::iota(yGrad.rbegin(), yGrad.rbegin() + centery, static_cast<double>(centerx) / 1.8f);
                }
            }
        }
    }  

    for (int row = 0; row < spaces.height(); ++row)
    {
        for (int col = 0; col < spaces.width(); ++col)
        {
            const double perlinContribution = noise.gen((col + 0.5) / spaces.width(), (row + 0.5) / spaces.height());
            const double gradientContribution = 0.5 * ((static_cast<double>(xGrad[col]) / centerx) + (static_cast<double>(yGrad[row]) / centery));
            if ( (P_WEIGHT * perlinContribution + G_WEIGHT * gradientContribution) > THRESHOLD)
            {
                spaces.at(col, row) = 1;
            }
        }
    }

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

    const int LARGE_PRIME = 198491317;
    const int seed = rng::noise(( coordinate.x + (LARGE_PRIME * coordinate.y) ));
    spatial::Grid<int> mask{ spatial::CHUNK_WIDTH, spatial::CHUNK_HEIGHT };
    determineAvailableSpaces(determineOpenFaces(coordinate), mask, seed);

    for (auto& [category, entityList] : biome.prototypes)
    {
        for (auto& [entity, algorithm, params] : entityList)
        {
           const auto positions = gen::String2Algorithm.at(algorithm)( seed, mask, params);
           gen::place(entity, spatial::toTileSpace(coordinate), positions, registry);
        }
    }
}

drft::gen::BiomeType drft::gen::WorldGenerator::getBiomeType(sf::Vector2i coordinate) const
{
    if (_cachedBiomeTypes.contains({ coordinate.x, coordinate.y })) {
        return _cachedBiomeTypes.at({ coordinate.x, coordinate.y });
    }
    auto dCoord = convertIntergerCoordinates(spatial::toTileSpace(coordinate));
    double t_noise = _temperatureNoise->gen(dCoord.x, dCoord.y, 0);
    double a_noise = _altitudeNoise->gen(dCoord.x, dCoord.y, 0);
    double m_noise = _moistureNoise->gen(dCoord.x, dCoord.y, 0);

    auto biomeType = determineBiomeType(t_noise, a_noise, m_noise);
    _cachedBiomeTypes[std::pair(coordinate.x, coordinate.y)] = biomeType;

    return biomeType;
}
