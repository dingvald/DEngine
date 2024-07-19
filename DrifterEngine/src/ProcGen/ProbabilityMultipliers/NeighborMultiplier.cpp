#include "pch.h"
#include "NeighborMultiplier.h"
#include <Spatial/Helpers.h>

void drft::gen::NeighborMultiplier::_createFromJSON(const rapidjson::Value& json)
{
    _tag = entt::hashed_string{ json["tag"].GetString() };
    _max = json["max"].GetInt();
}

drft::gen::ProbabilityMultiplier::HitCount drft::gen::NeighborMultiplier::_test(sf::Vector2i position, GenerationContext& context) const
{
    HitCount result = 0;
    const auto neighbors = spatial::getAdjacentPoints(position);
    for (auto&& neighbor : neighbors)
    {
        if (!context.grid.at(neighbor.x, neighbor.y).contains(_tag)) continue;

        result++;
        if (result >= _max) break;
    }
    return result;
}
