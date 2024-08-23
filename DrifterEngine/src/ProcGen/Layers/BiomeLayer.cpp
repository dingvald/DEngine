#include "pch.h"
#include "BiomeLayer.h"
#include "GenericLayers/PerlinNoiseLayer.h"
#include <ProcGen/Layers/VoronoiLayer.h>

using namespace entt::literals;

static const std::filesystem::path BIOME_FOLDER_PATH = "./data/static/biomes";

GenerationState BiomeLayerChunk::generate()
{
    const auto paddedBounds = addPaddingToBounds({ bounds().width, bounds().height });
    auto voronoiLayer = generateDependency<VoronoiLayer>(paddedBounds);
    if (voronoiLayer.state != GenerationState::Complete) return voronoiLayer.state;




    return GenerationState::Complete;
}


BiomeLayer::BiomeLayer()
    : GenerationLayer({64, 64})
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

const Biome& BiomeLayer::getBiomeAt(sf::Vector2i tilePosition)
{
    return _biomes.get("Forest");
}

const std::unordered_set<std::string>& BiomeLayer::getClimateTypes() const
{
    return _biomeClimateTypes;
}


