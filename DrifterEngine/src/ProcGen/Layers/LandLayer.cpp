#include "pch.h"
#include "LandLayer.h"
#include <ProcGen/Layers/VoronoiLayer.h>
#include <Spatial/Helpers.h>

using namespace entt::literals;

GenerationState LandLayerChunk::generate()
{
    auto paddedBounds = addPaddingToBounds({ bounds().width, bounds().height });
    auto voronoiLayer = generateDependency<VoronoiLayer>(paddedBounds);
    auto inputLayer = generateDependency<GenericLayer>(myLayer().getInputLayerID(), bounds());

    auto state = combinedState({ voronoiLayer.state, inputLayer.state });
    if (state != GenerationState::Complete) return state;

    for (auto&& point : voronoiLayer.instance->getCentroidsInBounds(paddedBounds))
    {
        double val = inputLayer.instance->getValueAt(point);
        if (val >= myLayer().getLandThreshold())
        {
            landPoints.push_back(point);
        }
        else
        {
            oceanPoints.push_back(point);
        }
    }

    return GenerationState::Complete;
}

LandLayer::LandLayer()
    : GenerationLayer({64, 64})
{
}

void LandLayer::createFromJson(const rapidjson::Value& json)
{
    if (json.HasMember("input_layer"))
    {
        _inputLayerID = entt::hashed_string{ json["input_layer"].GetString() };
    }
    if (json.HasMember("threshold"))
    {
        _landThreshold = json["threshold"].GetFloat();
    }
}

entt::id_type LandLayer::getInputLayerID() const
{
    return _inputLayerID;
}

float LandLayer::getLandThreshold() const
{
    return _landThreshold;
}

bool LandLayer::isLand(sf::Vector2i point)
{
    if (auto chunk = tryGetChunk(point))
    {
        auto closestLandPoint = drft::spatial::findClosestPoint(point, chunk->landPoints);
        auto closestOceanPoint = drft::spatial::findClosestPoint(point, chunk->oceanPoints);

        if (!closestLandPoint.has_value() && !closestOceanPoint.has_value()) return false;

        if (!closestOceanPoint.has_value())
        {
            return true;
        }
        else if (closestLandPoint.has_value())
        {
            const float oceanDistance = drft::spatial::distance(point, closestOceanPoint.value());
            const float landDistance = drft::spatial::distance(point, closestLandPoint.value());
            return (oceanDistance > landDistance);
        }
    }
    return false;
}
