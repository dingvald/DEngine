#pragma optimize("", off)

#include "pch.h"
#include "PoissonDiskLayer.h"
#include <Random/Random.h>
#include <jc_voronoi/jc_voronoi_adaptor.h>

constexpr float MaxDensity = 0.8f;

constexpr int Level1Gen = 1;
constexpr int Level2Gen = 2;

GenerationState PoissonDiskLayerChunk::generate(int level)
{
    switch (level)
    {
    case Level1Gen:
        return generateRandomPoints(_bounds);
    case Level2Gen:
        return relaxRandomPoints(_bounds);
    default:
        break;
    }

    return GenerationState::Complete;
}

GenerationState PoissonDiskLayerChunk::generateRandomPoints(sf::IntRect area)
{
    drft::rng::Random random{ getLocalSeed() };

    int numPoints = area.width * area.height * MaxDensity * _layer.getDistributionDensity();
    auto shuffledPositions = random.shuffleRect(area);

    int i = 0;
    for (auto&& position : shuffledPositions)
    {
        if (i >= numPoints) break;
        randomPoints.push_back(position);
        i++;
    }

    return GenerationState::Complete;
}

GenerationState PoissonDiskLayerChunk::relaxRandomPoints(sf::IntRect area)
{
    auto state = _layer.generateNeighborChunks(_index, { .layers = _layerManager, .desiredLevel = Level1Gen, .seed = getGlobalSeed() });
    if (state != GenerationState::Complete) return state;

    if (randomPoints.empty())
    {
        error_logger << "Error: random points empty at pos (" << _index.x << ", " << _index.y << ") in Poisson Disk layer." << std::endl;
        return GenerationState::Complete;
    }

    std::vector<sf::Vector2i> surroundingRandomPoints{ randomPoints.begin(), randomPoints.end() };

    _layer.forEachLoadedNeighborChunk(_index, [&surroundingRandomPoints](const PoissonDiskLayerChunk& chunk) {
        surroundingRandomPoints.insert(surroundingRandomPoints.end(), chunk.randomPoints.begin(), chunk.randomPoints.end() );
    });

    VoronoiDiagram diagram{ surroundingRandomPoints };
    diagram.forEachRelaxedPoint([this](sf::Vector2i point) {
        if (!_bounds.contains(point)) return;
        distributedPoints.insert(point);
    });

    return GenerationState::Complete;
}
PoissonDiskLayer::PoissonDiskLayer()
    : GenerationLayer({16, 16})
{}

double PoissonDiskLayer::getValueAt(sf::Vector2i position)
{
    if (auto chunk = tryGetChunk(position))
    {
        if (chunk->distributedPoints.contains(position))
        {
            return 1.0;
        }
    }
    return 0.0;
}

void PoissonDiskLayer::createFromJson(const rapidjson::Value& json)
{
    if (json.HasMember("density"))
    {
        _density = json["density"].GetFloat();
    }
}

float PoissonDiskLayer::getDistributionDensity() const
{
    return _density;
}
