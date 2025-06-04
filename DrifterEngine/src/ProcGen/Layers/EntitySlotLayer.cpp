#include "pch.h"
#include "EntitySlotLayer.h"
#include <ProcGen/Layers/BiomeLayer.h>
#include <Random/Random.h>

using namespace entt::literals;

GenerationState drft::EntitySlotLayerChunk::generate(GenerationLevel desiredLevel)
{
    auto biomeLayer = generateDependency<BiomeLayer>(_volume);
    if (!biomeLayer.isReady()) return biomeLayer.getState();

    // Collect dependencies
    std::unordered_set<entt::id_type> dependencies;
    biomeLayer.unwrap().forEachBiomeInArea(_volume.flatten(), _volume.min.z, 
        [&dependencies](const Biome* biome) {
            auto deps = biome->getEntitySlotDependencyIds();
            dependencies.insert(deps.begin(), deps.end());
        });

    // Generate dependencies
    GeneratedDependencies generatedDependencies;
    for (auto&& dependencyId : dependencies)
    {
        auto layer = generateDependency<IGetValueAtLayer>(dependencyId, _volume);
        if (!layer.isReady()) return layer.getState();

        generatedDependencies.emplace(dependencyId, &layer.unwrap());
    }

    // Detemine entity slot for each tile position
    rng::Random random = { getLocalSeed() };
    auto& canvas = _layer.getLayerManager().getCanvas("slot_canvas"_hs);
    spatial::forEachPointInRect(_volume.flatten(), 
        [&, z = _volume.min.z](sf::Vector2i point) {
            const sf::Vector3i position = { point.x, point.y, z };
            SlotDeterminer::DependencyValues values;
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

sf::Vector3i drft::EntitySlotLayer::getChunkDimensions() const
{
    return { 8, 8, 8 };
}
