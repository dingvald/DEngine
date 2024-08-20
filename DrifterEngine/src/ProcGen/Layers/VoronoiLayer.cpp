#include "pch.h"
#include "VoronoiLayer.h"
#include <ProcGen/Layers/JitteredGridLayer.h>
#include <jc_voronoi/jc_voronoi.h>

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

GenerationState VoronoiLayerChunk::generate()
{
    const auto paddedBounds = addPaddingToBounds({bounds().width, bounds().height});
    auto jitterLayer = generateDependency<JitteredGridLayer>(paddedBounds);
    if (jitterLayer.state != GenerationState::Complete) return jitterLayer.state;

    auto points = jitterLayer.instance->getPointsInBounds(paddedBounds);
    std::vector<jcv_point> jcv_points;
    jcv_points.reserve(points.size());
    for (auto&& point : points)
    {
        jcv_points.push_back(vector2i2jcvPoint(point));
        if (bounds().contains(point))
        {
            centroids.push_back(point);
        }  
    }

    jcv_diagram diagram;
    memset(&diagram, 0, sizeof(jcv_diagram));
    jcv_diagram_generate(jcv_points.size(), jcv_points.data(), nullptr, nullptr, &diagram);
   
    // edges
    const jcv_edge* edge = jcv_diagram_get_edges(&diagram);
    while (edge)
    {
        auto point1 = jcvPoint2Vector2i(edge->pos[0]);
        auto point2 = jcvPoint2Vector2i(edge->pos[1]);
        if (bounds().contains(point1))
        {
            edges.emplace_back(std::make_pair(point1, point2));
        }
        edge = jcv_diagram_get_next_edge(edge);
    }

    jcv_diagram_free(&diagram);

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
