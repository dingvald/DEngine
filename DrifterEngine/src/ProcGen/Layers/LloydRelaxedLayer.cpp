#pragma optimize("", off)

#include "pch.h"
#include <ProcGen/Layers/LloydRelaxedLayer.h>
#include <Random/Random.h>
#include <jc_voronoi/jc_voronoi_adaptor.h>

constexpr float MaxDensity = 0.8f;

constexpr int Level1Gen = 1;
constexpr int Level2Gen = 2;
constexpr int Level3Gen = 3;

GenerationState LloydRelaxedLayerChunk::generate(int level)
{
    switch (level)
    {
    case Level1Gen:
        return generateRandomPoints(_bounds);
    case Level2Gen:
        return collectNeighborPoints(_bounds);
    case Level3Gen:
        return applyRelaxationToPoints(_bounds);
    default:
        break;
    }

    return GenerationState::Complete;
}

GenerationState LloydRelaxedLayerChunk::generateRandomPoints(sf::IntRect area)
{
    drft::rng::Random random{ getLocalSeed() };

    int numPoints = area.width * area.height * MaxDensity * _layer.getDistributionDensity();
    auto shuffledPositions = random.shuffleRect(area);

    int i = 0;
    for (auto&& position : shuffledPositions)
    {
        if (i >= numPoints) break;
        randomPoints.insert(position);
        i++;
    }

    return GenerationState::Complete;
}

GenerationState LloydRelaxedLayerChunk::collectNeighborPoints(sf::IntRect area)
{
    auto state = _layer.generateNeighborChunks(_index, { .layers = _layerManager, .desiredLevel = Level1Gen, .seed = getGlobalSeed() });
    if (state != GenerationState::Complete) return state;

    neighborPoints.insert(randomPoints.begin(), randomPoints.end() );

    _layer.forEachLoadedNeighborChunk(_index, [this](const LloydRelaxedLayerChunk& chunk) {
        neighborPoints.insert(chunk.randomPoints.begin(), chunk.randomPoints.end());
        });

    return GenerationState::Complete;
}

GenerationState LloydRelaxedLayerChunk::applyRelaxationToPoints(sf::IntRect area)
{
    auto state = _layer.generateNeighborChunks(_index, { .layers = _layerManager, .desiredLevel = Level2Gen, .seed = getGlobalSeed() });
    if (state != GenerationState::Complete) return state;

    std::vector<sf::Vector2i> pointsToRelax{ neighborPoints.begin(), neighborPoints.end() };

    _layer.forEachLoadedNeighborChunk(_index, [&pointsToRelax](const LloydRelaxedLayerChunk& chunk) {
        pointsToRelax.insert(pointsToRelax.end(), chunk.neighborPoints.begin(), chunk.neighborPoints.end() );
    });

    for (int i = 0; i < _layer.getNumberOfRelaxationPasses() - 1; i++)
    {
        VoronoiDiagram diagram{ pointsToRelax };
        pointsToRelax = diagram.getRelaxedPoints();
    }

    VoronoiDiagram diagram{ pointsToRelax };
    diagram.forEachRelaxedPoint([this](sf::Vector2i point) {
        if (!_bounds.contains(point)) return;
        distributedPoints.insert(point);
    });

    return GenerationState::Complete;
}
LloydRelaxedLayer::LloydRelaxedLayer()
    : GenerationLayer({16, 16})
{}

double LloydRelaxedLayer::getValueAt(sf::Vector2i position)
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

void LloydRelaxedLayer::createFromJson(const rapidjson::Value& json)
{
    if (json.HasMember("density"))
    {
        _density = json["density"].GetFloat();
    }
    if (json.HasMember("passes"))
    {
        _passes = json["passes"].GetInt();
    }
}

float LloydRelaxedLayer::getDistributionDensity() const
{
    return _density;
}

int LloydRelaxedLayer::getNumberOfRelaxationPasses() const
{
    return _passes;
}
