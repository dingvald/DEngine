#include "pch.h"
#include "NoiseLayer.h"


void drft::rng::NoiseLayer::setDimensions(sf::Vector2i dimensions)
{
    _dimensions = dimensions;
}

void drft::rng::NoiseLayer::setNoise(PerlinNoise noise)
{
    _noise = noise;
}

double drft::rng::NoiseLayer::at(sf::Vector2i tilePosition) const
{
    double x_normalized = static_cast<double>(_dimensions.x) / static_cast<double>(tilePosition.x);
    double y_normalized = static_cast<double>(_dimensions.y) / static_cast<double>(tilePosition.y);
    double result = _noise.gen(x_normalized, y_normalized);
    for (auto&& func : _biasingFunctions)
    {
        result = func(result, tilePosition);
    }

    return result;
}

void drft::rng::NoiseLayer::addBiasingFunction(BiasingFunction&& func)
{
    _biasingFunctions.emplace_back(func);
}
