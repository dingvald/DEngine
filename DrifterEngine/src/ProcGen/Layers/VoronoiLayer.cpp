#include "pch.h"
#include "VoronoiLayer.h"
#include <ProcGen/Layers/JitteredGridLayer.h>
#include <jc_voronoi/jc_voronoi_adaptor.h>

using namespace drft;

GenerationState VoronoiLayerChunk::generate(int level)
{
    const auto paddedVolume = addPaddingToVolume({ _volume.dimensions().x, _volume.dimensions().y, 0 });
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

VoronoiLayer::VoronoiLayer()
    : GenerationLayer({128, 128, 8})
{
}

std::vector<sf::Vector3i> VoronoiLayer::getCentroidsInArea(sf::IntRect area, sf::Vector3i origin)
{
    std::vector<sf::Vector3i> result;

    forEachLoadedChunkInArea(area, origin, [&origin, &area, &result](VoronoiLayerChunk& chunk)
        {
            for (auto&& centroid : chunk.centroids)
            {
                if (!area.contains(centroid)) continue;
                result.emplace_back(centroid.x, centroid.y, origin.z);
            }
        });
    return result;
}

std::vector<std::pair<sf::Vector3i, sf::Vector3i>> VoronoiLayer::getEdgesInArea(sf::IntRect area, sf::Vector3i origin)
{
    std::vector<std::pair<sf::Vector3i, sf::Vector3i>> result;
    forEachLoadedChunkInArea(area, origin, [&origin, &area, &result](VoronoiLayerChunk& chunk)
        {
            for (auto&& edge : chunk.edges)
            {
                if (!area.contains(edge.first)) continue;
                sf::Vector3i pt1 = { edge.first.x, edge.first.y, origin.z };
                sf::Vector3i pt2 = { edge.second.x, edge.second.y, origin.z };
                result.emplace_back(pt1, pt2);
            }
        });
    return result;
}
