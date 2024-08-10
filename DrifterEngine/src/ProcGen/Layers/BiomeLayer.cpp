#include "pch.h"
#include "BiomeLayer.h"

GenerationState details::BiomeLayerChunk::generate()
{
    message = std::string{ "BiomeLayerChunk generated at (" } + std::to_string(bounds().left) + ", " + std::to_string(bounds().top) + ")";
    return GenerationState::Complete;
}

void details::BiomeLayerChunk::destroy()
{

}

BiomeLayer::BiomeLayer()
{
    setChunkDimensions({ 64, 64 });
}

std::string BiomeLayer::getMessagesInArea(sf::IntRect area)
{
    std::string message;
    forEachLoadedChunkInArea(area, [&message](details::BiomeLayerChunk& chunk)
        {
            message += std::string{ "\n" + chunk.message };
        });

    return message;
}

