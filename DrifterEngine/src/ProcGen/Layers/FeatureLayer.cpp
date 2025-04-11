#include "pch.h"
#include "FeatureLayer.h"
#include <ProcGen/Layers/BiomeLayer.h>
#include <Random/Random.h>

using namespace entt::literals;

GenerationState drft::FeatureLayerChunk::generate(GenerationLevel)
{
    auto biomes = generateDependency<BiomeLayer>(addPaddingToVolume({ _volume.dimensions().x, _volume.dimensions().y, 0 }));
    if (!biomes.isReady()) return biomes.getState();

    rng::Random random = { getLocalSeed() };
    auto randomPoint = random.positionInRect(_volume.flatten());
    const sf::Vector3i randomPoint3d = { randomPoint.x, randomPoint.y, _volume.min.z };

    if (auto biome = biomes.unwrap().getBiomeAt(randomPoint3d))
    {
        Biome::DependencyValues dependencyValues;
        for (auto&& dependencyID : biome->getFeatureDependencyIds())
        {
            if (dependencyValues.contains(dependencyID)) continue;

            auto depLayer = generateDependency<IGetValueAtLayer>(dependencyID, _volume);
            if (!depLayer.isReady()) return depLayer.getState();

            dependencyValues.emplace(dependencyID, depLayer.unwrap().getValueAt(randomPoint3d));
        }

        auto featureId = biome->determineValidFeature(dependencyValues);
        if (!featureId.has_value()) return GenerationState::Complete;

        if (auto feature = _layer.getRegistries().features.get(featureId.value()))
        {
            auto result = feature->generate(FeatureGenerationContext{ getLocalSeed() });
            auto& entityCanvas = _layer.getLayerManager().getCanvas("entity_canvas"_hs);
            for (auto&& [entity, position] : result.entityPositions)
            {  
                entityCanvas.set(entity, randomPoint3d + position);
            }
        }
    }

    return GenerationState::Complete;
}

sf::Vector3i drft::FeatureLayer::getChunkDimensions() const
{
    return { 16, 16, 8 };
}
