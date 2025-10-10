#include "pch.h"
#include "FeatureLayer.h"
#include <Generation/Layers/BiomeLayer.h>
#include <Random/Random.h>
#include <Generation/GenerationContext.h>
#include <Spatial/Helpers.h>

#include <Generation/Layers/PositionArrayLayer.h>

using namespace entt::literals;

namespace Internal
{
    static sf::Vector3i getMinCenterPosition(const drft::spatial::AABBi& volume)
    {
        sf::Vector2i center2d = drft::spatial::toXY(volume.center());
        return sf::Vector3i{ center2d.x, center2d.y, volume.min.z };
    }
}

GenerationState drft::FeatureLayerChunk::generate(GenerationLevel level)
{
    switch (level)
    {
    case drft::GenerationLevel::One:
        return generateFeatures();
    case drft::GenerationLevel::Two:
        return generateSurroundingFeatures();
    default:
        break;
    }
    return GenerationState::Complete;
}

void drft::FeatureLayerChunk::placeFeature(const SlotPositionList& slots)
{
    for (auto&& [slot, position, priority] : slots)
    {
        if (!_volume.contains(position)) continue;

        const int finalPriority = priority == UNINITIALIZED_SLOT_PRIORITY ? 100 : priority;
        if (!this->slots.contains(position))
        {
            this->slots.emplace(position, SlotPriority{ slot, finalPriority });
            continue;
        }

        SlotPriority& slotPriority = this->slots.at(position);
        if (slotPriority.priority > finalPriority) continue;

        slotPriority.slot = slot;
        slotPriority.priority = finalPriority;
    }
}

GenerationState drft::FeatureLayerChunk::generateFeatures()
{
    auto biomes = generateDependency<BiomeLayer>(_volume);
    if (!biomes.isReady()) return biomes.getState();

    const sf::Vector3i biomePoint = Internal::getMinCenterPosition(_volume);

    const Biome* biome = biomes.unwrap().getBiomeAt(biomePoint);
    if (!biome) return GenerationState::Complete;

    SlotDeterminer::DependencyValues dependencyValues;
    for (auto&& dependencyID : biome->getFeatureDependencyIds())
    {
        if (dependencyValues.contains(dependencyID)) continue;

        auto depLayer = generateDependency<IGetValueAtLayer>(dependencyID, _volume);
        if (!depLayer.isReady()) return depLayer.getState();

        dependencyValues.emplace(dependencyID, depLayer.unwrap().getValueAt(biomePoint));
    }

    auto features = biome->determineValidFeatures(dependencyValues);
    for (auto&& featureId : features)
    {
        const IFeature* feature = _layer.getRegistries().features.get(featureId);
        if (!feature) continue;

        GenerationContext context = { getLocalSeed(), _layer.getRegistries() };
        GeneratedFeature generatedFeature = feature->generate(biomePoint, context);

        generatedFeatures.emplace_back(std::move(generatedFeature));
    }

    return GenerationState::Complete;
}

GenerationState drft::FeatureLayerChunk::generateSurroundingFeatures()
{
    spatial::AABBi expandedVolume = _volume.expand({ 30.f, 30.f, 1.f });
    auto featureLayer = generateDependency<FeatureLayer>(expandedVolume, GenerationLevel::One);
    if (!featureLayer.isReady()) return featureLayer.getState();

    // Remove overlapping features

    // Place features
    featureLayer.unwrap().forEachLoadedChunkInVolume(expandedVolume, 
        [this](FeatureLayerChunk& chunk) {
            for (auto&& feature : chunk.generatedFeatures)
            {
                placeFeature(feature.slotPositions);
            }
        });

    return GenerationState::Complete;
}

sf::Vector3i drft::FeatureLayer::getChunkDimensions() const
{
    return { 16, 16, 8 };
}
