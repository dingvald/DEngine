#include "pch.h"
#include "EntityLayer.h"
#include <ProcGen/Layers/BiomeLayer.h>

GenerationState drft::EntityLayerChunk::generate(GenerationLevel desiredLevel)
{
    auto biomes = generateDependency<BiomeLayer>(addPaddingToVolume({ _volume.dimensions().x, _volume.dimensions().y, 0 }));
    if (!biomes.isReady()) return biomes.getState();



    return GenerationState::Complete;
}

sf::Vector3i drft::EntityLayer::getChunkDimensions() const
{
    return sf::Vector3i();
}
