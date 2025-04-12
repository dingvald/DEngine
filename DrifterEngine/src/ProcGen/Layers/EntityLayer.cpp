#include "pch.h"
#include "EntityLayer.h"
#include <ProcGen/Layers/BiomeLayer.h>
#include <Random/Random.h>

using namespace entt::literals;

GenerationState drft::EntityLayerChunk::generate(GenerationLevel desiredLevel)
{
    auto biomeLayer = generateDependency<BiomeLayer>(_volume);
    if (!biomeLayer.isReady()) return biomeLayer.getState();

    std::unordered_set<entt::id_type> dependencies;
    biomeLayer.unwrap().forEachBiomeInArea(_volume.flatten(), _volume.min.z, 
        [&dependencies](const Biome* biome) {
            auto deps = biome->getEntitySlotDependencyIds();
            dependencies.insert(deps.begin(), deps.end());
        });

    GeneratedDependencies generatedDependencies;
    for (auto&& dependencyId : dependencies)
    {
        auto layer = generateDependency<IGetValueAtLayer>(dependencyId, _volume);
        if (!layer.isReady()) return layer.getState();

        generatedDependencies.emplace(dependencyId, &layer.unwrap());
    }

    rng::Random random = { getLocalSeed() };
    auto& canvas = _layer.getLayerManager().getCanvas("entity_canvas"_hs);
    spatial::forEachPointInRect(_volume.flatten(), 
        [&, z = _volume.min.z](sf::Vector2i point) {
            const sf::Vector3i position = { point.x, point.y, z };
            Biome::DependencyValues values;
            for (auto&& [id, dep] : generatedDependencies)
            {
                values.emplace(id, dep->getValueAt(position));
            }
            auto biome = biomeLayer.unwrap().getBiomeAt(position);
            if (!biome) return;
            auto entities = biome->determineValidEntitySlots(values);
            auto selection = random.randomSelection(entities);
            if (!selection) return;
            canvas.set(*selection, position);
        });

    return GenerationState::Complete;
}

sf::Vector3i drft::EntityLayer::getChunkDimensions() const
{
    return { 8, 8, 8 };
}
