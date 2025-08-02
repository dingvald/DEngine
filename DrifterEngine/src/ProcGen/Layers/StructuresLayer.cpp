#include "pch.h"
#include "StructuresLayer.h"
#include <Utility/StandardLogger.h>
#include <ProcGen/GenerationContext.h>

using namespace drft;

GenerationState drft::StructuresLayerChunk::generate(GenerationLevel)
{
    if (auto structure = _layer.tryGetStructureInVolume(_volume))
    {
        const int depth = _volume.min.z / _volume.dimensions().z;
        GenerationContext context = { getGlobalSeed(), _layer.getRegistries() };
        auto structureLayer = structure->getOrGenerateLayer(depth, context);
        if (!structureLayer)
        {
            LOG_ERROR("StructuresLayerChunk - could not generate requested layer");
            return GenerationState::Failed;
        }

        auto ifVolumeContainsPosition = [volume = _volume](sf::Vector3i pos) { return volume.contains(pos); };
        mergeSlotPositionMaps(slots, structureLayer->getEntitySlots(), ifVolumeContainsPosition);
    }
    return GenerationState::Complete;
}

void StructuresLayer::createFromJson(const rapidjson::Value& json)
{
    if (json.HasMember("structures"))
    {
        for (auto&& obj : json["structures"].GetObject())
        {
            auto structureName = obj.name.GetString();
            const StructureTemplate& structureTemplate = getRegistries().structures.get(entt::hashed_string{structureName});

            // TODO: determine position
            sf::Vector3i placementPosition = { 0,0,0 };
            //

            _structures.emplace_back(structureTemplate, placementPosition);
        }
    }
}

sf::Vector3i StructuresLayer::getChunkDimensions() const
{
    return { 64, 64, 8 };
}

StructureInstance* drft::StructuresLayer::tryGetStructureInVolume(const spatial::AABBi& volume)
{
    for (auto&& structure : _structures)
    {
        if (structure.getVolume().intersects(volume)) 
            return &structure;
    }
    return nullptr;
}
