#include "pch.h"
#include "PoissonDiskLayer.h"


GenerationState PoissonDiskLayerChunk::generate(int level)
{
    switch (level)
    {
    case 1:
        return stage1Generation(_bounds);
    case 2:
        return stage2Generation(_bounds);
    default:
        break;
    }

    return GenerationState::Complete;
}
GenerationState PoissonDiskLayerChunk::stage1Generation(sf::IntRect area)
{
    // Pick n number of points in proportion to density

    return GenerationState::Complete;
}
GenerationState PoissonDiskLayerChunk::stage2Generation(sf::IntRect area)
{
    auto state = _layer.generateNeighborChunks(_index, { .layers = _layerManager, .desiredLevel = 1, .seed = getGlobalSeed() });
    if (state != GenerationState::Complete) return state;



    return GenerationState::Complete;
}
PoissonDiskLayer::PoissonDiskLayer()
    : GenerationLayer({16, 16})
{}

double PoissonDiskLayer::getValueAt(sf::Vector2i position)
{
    return 1.0;
}

void PoissonDiskLayer::createFromJson(const rapidjson::Value& json)
{
    if (json.HasMember("density"))
    {
        _density = json["density"].GetFloat();
    }
}
