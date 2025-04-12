#include "pch.h"
#include "VoronoiLayer.h"
#include <ProcGen/Layers/JitteredGridLayer.h>
#include <jc_voronoi/jc_voronoi_adaptor.h>

using namespace drft;

GenerationState VoronoiLayerChunk::generate(GenerationLevel)
{
    const auto paddedVolume = _volume.expand({1.5f, 1.5f, 1.0f});
    auto jitterLayer = generateDependency<JitteredGridLayer>(paddedVolume);
    if (!jitterLayer.isReady()) return jitterLayer.getState();

    auto points = jitterLayer.unwrap().getPointsInArea(paddedVolume.flatten(), _volume.center());

    VoronoiDiagram diagram = { points };

    diagram.forEachSite([this](VoronoiSite site)
        {
            if (!_volume.contains2d(site.point)) return;
            centroids.push_back(site.point);
        });
    
    diagram.forEachEdge([this](Edge edge)
        {
            if (!_volume.contains2d(edge.first)) return;
            edges.emplace_back(std::move(edge));
        });

    return GenerationState::Complete;
}

std::vector<sf::Vector3i> VoronoiLayer::getCentroidsInArea(sf::IntRect area, int z)
{
    std::vector<sf::Vector3i> result;

    forEachLoadedChunkInArea(area, z, [z, &area, &result](VoronoiLayerChunk& chunk)
        {
            for (auto&& centroid : chunk.centroids)
            {
                if (!area.contains(centroid)) continue;
                result.emplace_back(centroid.x, centroid.y, z);
            }
        });
    return result;
}

std::vector<std::pair<sf::Vector3i, sf::Vector3i>> VoronoiLayer::getEdgesInArea(sf::IntRect area, int z)
{
    std::vector<std::pair<sf::Vector3i, sf::Vector3i>> result;
    forEachLoadedChunkInArea(area, z, [z, &area, &result](VoronoiLayerChunk& chunk)
        {
            for (auto&& edge : chunk.edges)
            {
                if (!area.contains(edge.first)) continue;
                sf::Vector3i pt1 = { edge.first.x, edge.first.y, z };
                sf::Vector3i pt2 = { edge.second.x, edge.second.y, z };
                result.emplace_back(pt1, pt2);
            }
        });
    return result;
}

sf::Vector3i drft::VoronoiLayer::getChunkDimensions() const
{
    return { 128, 128, 8 };
}
