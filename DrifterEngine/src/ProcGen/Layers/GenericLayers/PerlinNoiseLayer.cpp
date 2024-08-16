#include "pch.h"
#include "PerlinNoiseLayer.h"
#include <Utility/Math.h>

static const double EXPERIMENTALLY_DETERMINED_MIN = 0.25;
static const double EXPERIMENTALLY_DETERMINED_MAX = 0.75;

double PerlinNoiseLayer::getValueAt(sf::Vector2i tilePosition)
{
    double val = _getValueAt(tilePosition);
    val = drft::math::remap(EXPERIMENTALLY_DETERMINED_MIN, EXPERIMENTALLY_DETERMINED_MAX, -1.0, 1.0, val);
    return val;
}

void PerlinNoiseLayer::createFromJson(const rapidjson::Value& json)
{
    if (json.HasMember("resolution"))
    {
        _resolution = json["resolution"].GetFloat();
    }
    if (json.HasMember("octaves"))
    {
        _octaves = json["octaves"].GetInt();
    }
    if (json.HasMember("lacunarity"))
    {
        _lacunarity = json["lacunarity"].GetFloat();
    }
    if (json.HasMember("gain"))
    {
        _gain = json["gain"].GetFloat();
    }

    _noise = drft::rng::PerlinNoise{ 0, _octaves, _lacunarity, _gain};
}

double PerlinNoiseLayer::_getValueAt(sf::Vector2i tilePosition) const
{
    double x_normalized = static_cast<double>(tilePosition.x) / static_cast<double>(_dimensions.x) * _resolution;
    double y_normalized = static_cast<double>(tilePosition.y) / static_cast<double>(_dimensions.y) * _resolution;
    return _noise.gen(x_normalized, y_normalized);
}
