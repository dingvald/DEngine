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
static constexpr double TEMPERATURE_HOT = 0.57;

// Altitude cutoffs
static constexpr double ALTITUDE_LOW = 0.43;
static constexpr double ALTITUDE_HIGH = 0.57;

// Moisture cutoffs
static constexpr double MOISTURE_ARID = 0.43;
static constexpr double MOISTURE_HUMID = 0.57;

//--------------------------------------------

drft::gen::WorldGenerator::WorldGenerator()
{
    registerMachines();
}

void drft::gen::WorldGenerator::setSeed(unsigned int seed)
{
    _seed = seed;

    const unsigned int temperatureSeed = rng::noise(_seed);
    const unsigned int altitudeSeed = rng::noise(temperatureSeed);
    const unsigned int moistureSeed = rng::noise(altitudeSeed);

    _temperatureNoise = std::make_unique<rng::PerlinNoise>(temperatureSeed);
    _altitudeNoise = std::make_unique<rng::PerlinNoise>(altitudeSeed);
    _moistureNoise = std::make_unique<rng::PerlinNoise>(moistureSeed);
}

void drft::gen::WorldGenerator::registerMachines()
{

}

void drft::gen::WorldGenerator::generate(sf::Vector2i coordinate, entt::registry& registry) const
{
    // Always place tiles
    gen::fastFill("Tile", spatial::toTileSpace(coordinate), registry);
}
