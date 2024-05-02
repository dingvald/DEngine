#include "pch.h"
#include "NoiseLayer.h"
#include "Utility/Math.h"


drft::rng::NoiseLayer::NoiseLayer(sf::Vector2i dimensions, unsigned int seed)
    : _dimensions(dimensions)
    , _seed(seed)
{}

void drft::rng::NoiseLayer::createFromJSON(const rapidjson::Value& json)
{
    if (json.HasMember("Resolution"))
    {
        _resolution = json["Resolution"].GetInt();
    }
    if (json.HasMember("Octaves"))
    {
        _octaves = json["Octaves"].GetInt();
    }
    if (json.HasMember("Lacunarity"))
    {
        _lacunarity = json["Lacunarity"].GetFloat();
    }
    if (json.HasMember("Gain"))
    {
        _gain = json["Gain"].GetFloat();
    }

    _noise = rng::PerlinNoise{ _seed, _octaves, _lacunarity, _gain };

    // find min and max for remapping
    for (int y = 0; y < _dimensions.y; y += 32)
    {
        for (int x = 0; x < _dimensions.x; x += 32)
        {
            double val = _getValueAt({ x, y });
            _min = std::min(_min, val);
            _max = std::max(_max, val);
        }
    }
}

double drft::rng::NoiseLayer::getValueAt(sf::Vector2i tilePosition) const
{
    double val = _getValueAt(tilePosition);
    val = math::remap(_min, _max, 0.0, 1.0, val);
    for (auto&& func : _biasingFunctions)
    {
        val = func(val, tilePosition);
    }

    return val;
}

void drft::rng::NoiseLayer::addBiasingFunction(BiasingFunction&& func)
{
    _biasingFunctions.emplace_back(func);
}

double drft::rng::NoiseLayer::_getValueAt(sf::Vector2i tilePosition) const
{
    double x_normalized = static_cast<double>(tilePosition.x) / static_cast<double>(_dimensions.x) * _resolution;
    double y_normalized = static_cast<double>(tilePosition.y) / static_cast<double>(_dimensions.y) * _resolution;
    return _noise.gen(x_normalized, y_normalized);
}
