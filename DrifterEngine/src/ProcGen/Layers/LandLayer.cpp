#include "pch.h"
#include "LandLayer.h"
#include <ProcGen/Layers/VoronoiLayer.h>

using namespace entt::literals;

GenerationState LandLayerChunk::generate()
{
    auto paddedBounds = addPaddingToBounds({ bounds().width, bounds().height });
    auto voronoiLayer = generateDependency<VoronoiLayer>(paddedBounds);
    auto inputLayer = generateDependency<GenericLayer>("height"_hs, bounds());

    auto state = combinedState({ voronoiLayer.state, inputLayer.state });
    if (state != GenerationState::Complete) return state;

    for (auto&& point : voronoiLayer.instance->getCentroidsInBounds(paddedBounds))
    {
        double val = inputLayer.instance->getValueAt(point);
        if (val >= 0)
        {
            landPoints.insert(point);
        }
        else
        {
            oceanPoints.insert(point);
        }
    }

    return GenerationState::Complete;
}

LandLayer::LandLayer()
    : GenerationLayer({64, 64})
{
}

std::vector<sf::Vector2i> LandLayer::getLandPointsInBounds(sf::IntRect area)
{
    entt::dense_set<sf::Vector2i> visitedPoints;
    std::vector<sf::Vector2i> result;

    forEachLoadedChunkInArea(area, [&visitedPoints, &result](LandLayerChunk& chunk)
        {
            for (auto&& point : chunk.landPoints)
            {
                if (visitedPoints.contains(point)) continue;
                visitedPoints.insert(point);
                result.push_back(point);
            }
        });

    return result;
}

std::vector<sf::Vector2i> LandLayer::getOceanPointsInBounds(sf::IntRect area)
{
    entt::dense_set<sf::Vector2i> visitedPoints;
    std::vector<sf::Vector2i> result;

    forEachLoadedChunkInArea(area, [&visitedPoints, &result](LandLayerChunk& chunk)
        {
            for (auto&& point : chunk.oceanPoints)
            {
                if (visitedPoints.contains(point)) continue;
                visitedPoints.insert(point);
                result.push_back(point);
            }
        });

    return result;
}

entt::id_type LandLayer::getInputLayerID() const
{
    return _inputLayerID;
}