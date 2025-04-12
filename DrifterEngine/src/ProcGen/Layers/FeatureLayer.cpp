#include "pch.h"
#include "FeatureLayer.h"
#include <ProcGen/Layers/BiomeLayer.h>
#include <Random/Random.h>

using namespace entt::literals;

GenerationState drft::FeatureLayerChunk::generate(GenerationLevel level)
{
    switch (level)
    {
    case drft::GenerationLevel::One:
        return selectFeature();
        break;
    case drft::GenerationLevel::Two:
        return checkNeighbors();
        break;
    case drft::GenerationLevel::Three:
        return placeFeature();
        break;
    default:
        break;
    }
    return GenerationState::Complete;
}

GenerationState drft::FeatureLayerChunk::selectFeature()
{
    auto biomes = generateDependency<BiomeLayer>(_volume);
    if (!biomes.isReady()) return biomes.getState();

    rng::Random random = { getLocalSeed() };
    auto randomPoint = random.positionInRect(_volume.expand({ 0.8f, 0.8f, 1.0f }).flatten());
    const sf::Vector3i randomPoint3d = { randomPoint.x, randomPoint.y, _volume.min.z };

    auto biome = biomes.unwrap().getBiomeAt(randomPoint3d);
    if (!biome) return GenerationState::Complete;

    Biome::DependencyValues dependencyValues;
    for (auto&& dependencyID : biome->getFeatureDependencyIds())
    {
        if (dependencyValues.contains(dependencyID)) continue;

        auto depLayer = generateDependency<IGetValueAtLayer>(dependencyID, _volume);
        if (!depLayer.isReady()) return depLayer.getState();

        dependencyValues.emplace(dependencyID, depLayer.unwrap().getValueAt(randomPoint3d));
    }

    auto features = biome->determineValidFeatures(dependencyValues);
    auto selectedFeature = random.randomSelection(features);
    if (!selectedFeature) return GenerationState::Complete;

    if (auto feature = _layer.getRegistries().features.get(*selectedFeature))
    {
        optionalGeneratedFeature = feature->generate(FeatureGenerationContext{ getLocalSeed(), _layer.getRegistries() });
        optionalGeneratedFeature->area.position += randomPoint;
    }

    return GenerationState::Complete;
}

GenerationState drft::FeatureLayerChunk::checkNeighbors()
{
    auto state = generateNeighborChunks2d(GenerationLevel::One);
    if (state != GenerationState::Complete) return state;

    if (!optionalGeneratedFeature.has_value()) return GenerationState::Complete;

    forEachLoadedNeighborChunk2d([&](const FeatureLayerChunk& chunk) {
            if (!optionalGeneratedFeature.has_value()) return;
            if (!chunk.optionalGeneratedFeature.has_value()) return;

            auto intersection = chunk.optionalGeneratedFeature->area.findIntersection(optionalGeneratedFeature->area);
            if (intersection)
            {
                LOG_MSG("Features overlapping fyi");
            }
        });

    return GenerationState::Complete;
}

GenerationState drft::FeatureLayerChunk::placeFeature()
{
    if (!optionalGeneratedFeature.has_value()) return GenerationState::Complete;

    auto& canvas = _layer.getLayerManager().getCanvas("entity_canvas"_hs);
    for (auto&& [entity, position] : optionalGeneratedFeature->entityPositions)
    {
        sf::Vector3i globalPosition = spatial::vec3FromPlanar(optionalGeneratedFeature->area.position) + position;
        canvas.forceSet(entity, globalPosition);
    }
    return GenerationState::Complete;
}

sf::Vector3i drft::FeatureLayer::getChunkDimensions() const
{
    return { 16, 16, 8 };
}
