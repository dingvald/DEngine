#include "pch.h"
#include "BiomeLayer.h"
#include "GenericLayers/PerlinNoiseLayer.h"

using namespace entt::literals;

static const std::filesystem::path BIOME_FOLDER_PATH = "./data/static/biomes";

GenerationState BiomeLayerChunk::generate()
{
    




    return GenerationState::Complete;
}


BiomeLayer::BiomeLayer()
    : GenerationLayer({8, 8})
{
    _biomes.createBiomesFromJSON(BIOME_FOLDER_PATH);
    _biomes.forEachBiome([this](const std::string& name, const Biome& biome)
        {
            for (auto&& [name, _] : biome.getClimateRanges())
            {
                _biomeClimateTypes.insert(name);
            }
        });
}

const std::unordered_set<std::string>& BiomeLayer::getClimateTypes() const
{
    return _biomeClimateTypes;
}


