#include "pch.h"
#include <ProcGen/Layers/LloydRelaxedLayer.h>
#include <Random/Random.h>
#include <jc_voronoi/jc_voronoi_adaptor.h>

using namespace drft;

constexpr float MaxDensity = 0.8f;

GenerationState LloydRelaxedLayerChunk::generate(GenerationLevel desiredLevel)
{
    switch (desiredLevel)
    {
    case GenerationLevel::One:
        return generateRandomPoints(_volume);
    case GenerationLevel::Two:
        return collectNeighborPoints(_volume);
    case GenerationLevel::Three:
        return applyRelaxationToPoints(_volume);
    default:
        break;
    }

    return GenerationState::Complete;
}

GenerationState LloydRelaxedLayerChunk::generateRandomPoints(spatial::AABB<int> volume)
{
    drft::rng::Random random{ getLocalSeed() };

    int numPoints = volume.dimensions().x * volume.dimensions().y * MaxDensity * _layer.getDistributionDensity();
    auto shuffledPositions = random.shuffleRect(volume.flatten());

    int i = 0;
    for (auto&& position : shuffledPositions)
    {
        if (i >= numPoints) break;
        randomPoints.insert(position);
        i++;
    }

    return GenerationState::Complete;
}

GenerationState LloydRelaxedLayerChunk::collectNeighborPoints(spatial::AABB<int> volume)
{
    auto state = generateNeighborChunks2d(GenerationLevel::One);
    if (state != GenerationState::Complete) return state;

    neighborPoints.insert(randomPoints.begin(), randomPoints.end() );

    forEachLoadedNeighborChunk2d([this](const LloydRelaxedLayerChunk& chunk) {
        neighborPoints.insert(chunk.randomPoints.begin(), chunk.randomPoints.end());
        });

    return GenerationState::Complete;
}

GenerationState LloydRelaxedLayerChunk::applyRelaxationToPoints(spatial::AABB<int> volume)
{
    auto state = generateNeighborChunks2d(GenerationLevel::Two);
    if (state != GenerationState::Complete) return state;

    std::vector<sf::Vector2i> pointsToRelax{ neighborPoints.begin(), neighborPoints.end() };

    forEachLoadedNeighborChunk2d([&pointsToRelax](const LloydRelaxedLayerChunk& chunk) {
        pointsToRelax.insert(pointsToRelax.end(), chunk.neighborPoints.begin(), chunk.neighborPoints.end() );
    });

    for (int i = 0; i < _layer.getNumberOfRelaxationPasses() - 1; i++)
    {
        VoronoiDiagram diagram{ pointsToRelax };
        pointsToRelax = diagram.getRelaxedPoints();
    }

    VoronoiDiagram diagram{ pointsToRelax };
    diagram.forEachRelaxedPoint([this](sf::Vector2i point) {
        if (!_volume.contains2d(point)) return;
        distributedPoints.insert(point);
    });

    return GenerationState::Complete;
}

double LloydRelaxedLayer::getValueAt(sf::Vector3i position)
{
    if (const auto chunk = tryGetChunk(position))
    {
        if (chunk->distributedPoints.contains(spatial::toXY(position)))
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

sf::Vector3i drft::LloydRelaxedLayer::getChunkDimensions() const
{
    return { 8, 8, 8 };
}
