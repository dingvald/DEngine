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
        return generateFeatures();
        break;
    case drft::GenerationLevel::Two:
        return checkNeighbors();
        break;
    case drft::GenerationLevel::Three:
        return placeFeatures();
        break;
    default:
        break;
    }
    return GenerationState::Complete;
}

GenerationState drft::FeatureLayerChunk::generateFeatures()
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
    for (auto&& featureId : features)
    {
        auto feature = _layer.getRegistries().features.get(featureId);
        if (!feature) continue;

        FeatureGenerationResult generatedFeature = feature->generate(FeatureGenerationContext{ getLocalSeed(), _layer.getRegistries() });
        generatedFeature.area.position += randomPoint;
        generatedFeatures.push_back(std::move(generatedFeature));
    }

    return GenerationState::Complete;
}

GenerationState drft::FeatureLayerChunk::checkNeighbors()
{
    auto state = generateNeighborChunks2d(GenerationLevel::One);
    if (state != GenerationState::Complete) return state;

    if (generatedFeatures.empty()) return GenerationState::Complete;

    forEachLoadedNeighborChunk2d([&](const FeatureLayerChunk& chunk) {
            if (generatedFeatures.empty()) return;
            if (!chunk.generatedFeatures.empty()) return;

            // TODO: Handle overlapping with neighbor features
        });

    return GenerationState::Complete;
}

GenerationState drft::FeatureLayerChunk::placeFeatures()
{
    if (generatedFeatures.empty()) return GenerationState::Complete;

    auto& canvas = _layer.getLayerManager().getCanvas("entity_canvas"_hs);
    for (auto&& generatedFeature : generatedFeatures)
    {
        for (auto&& [entity, position] : generatedFeature.entityPositions)
        {
            sf::Vector3i globalPosition = spatial::vec3FromPlanar(generatedFeature.area.position) + position;
            canvas.forceSet(entity, globalPosition);
        }
    }
    
    return GenerationState::Complete;
}

sf::Vector3i drft::FeatureLayer::getChunkDimensions() const
{
    return { 16, 16, 8 };
}
