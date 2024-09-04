#include "pch.h"
#include "VoronoiLayer.h"
#include <ProcGen/Layers/JitteredGridLayer.h>
#include <jc_voronoi/jc_voronoi_adaptor.h>

namespace
{
    sf::Vector2i jcvPoint2Vector2i(jcv_point point)
    {
        sf::Vector2i result;
        result.x = point.x;
        result.y = point.y;
        return result;
    }
    jcv_point vector2i2jcvPoint(sf::Vector2i point)
    {
        return jcv_point{ .x = static_cast<float>(point.x), .y = static_cast<float>(point.y) };
    }
}

GenerationState VoronoiLayerChunk::generate(int level)
{
    const auto paddedBounds = addPaddingToBounds({_bounds.width, _bounds.height});
    auto jitterLayer = generateDependency<JitteredGridLayer>(paddedBounds);
    if (!jitterLayer.isReady()) return jitterLayer.getState();

    auto points = jitterLayer.unwrap().getPointsInBounds(paddedBounds);

    VoronoiDiagram diagram = { points };

    diagram.forEachSite([this](VoronoiSite site)
        {
            if (!_bounds.contains(site.point)) return;
            centroids.push_back(site.point);
        });
    
    diagram.forEachEdge([this](Edge edge)
        {
            if (!_bounds.contains(edge.first)) return;
            edges.emplace_back(std::move(edge));
        });

    return GenerationState::Complete;
}

VoronoiLayer::VoronoiLayer()
    : GenerationLayer({256, 256})
{
}

std::vector<sf::Vector2i> VoronoiLayer::getCentroidsInBounds(sf::IntRect area)
{
    std::vector<sf::Vector2i> result;
    forEachLoadedChunkInArea(area, [&area, &result](VoronoiLayerChunk& chunk)
        {
            for (auto&& centroid : chunk.centroids)
            {
                if (!area.contains(centroid)) continue;
                result.push_back(centroid);
            }
        });
    return result;
}

std::vector<std::pair<sf::Vector2i, sf::Vector2i>> VoronoiLayer::getEdgesInBounds(sf::IntRect area)
{
    std::vector<std::pair<sf::Vector2i, sf::Vector2i>> result;
    forEachLoadedChunkInArea(area, [&area, &result](VoronoiLayerChunk& chunk)
        {
            for (auto&& edge : chunk.edges)
            {
                if (!area.contains(edge.first)) continue;
                result.push_back(edge);
            }
        });
    return result;
}
