#include "pch.h"
#include "NoiseMap.h"
#include "PerlinNoise.h"
#include "Utility/Math.h"


drft::spatial::Grid<double> drft::rng::NoiseMap::generate(sf::Vector2i dimensions, sf::Vector2i repetitions, int seed, int octaves, float lacunarity, float gain)
{
    PerlinNoise noise(seed, octaves, lacunarity, gain);
    spatial::Grid<double> result(dimensions.x, dimensions.y);
    double min = DBL_MAX;
    double max = DBL_MIN;

    for (int y = 0; y < result.height(); ++y)
    {
        for (int x = 0; x < result.width(); ++x)
        {
            double x_normalized = static_cast<double>(x) / static_cast<double>(result.width() / repetitions.x);
            double y_normalized = static_cast<double>(y) / static_cast<double>(result.height() / repetitions.y);
            double val = noise.gen(x_normalized, y_normalized);
            min = std::min(min, val);
            max = std::max(max, val);
            result.at(x, y) = val;
        }
    }

    // Normalize output between 0 - 1
    for (int y = 0; y < result.height(); ++y)
    {
        for (int x = 0; x < result.width(); ++x)
        {
            result.at(x, y) = math::inverseLerp(min, max, result.at(x, y));
        }
    }

    return result;
}
