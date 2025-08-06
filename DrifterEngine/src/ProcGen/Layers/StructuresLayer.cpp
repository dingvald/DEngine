#include "pch.h"
#include "StructuresLayer.h"
#include <Utility/StandardLogger.h>
#include <ProcGen/GenerationContext.h>
#include <Spatial/Helpers.h>

using namespace drft;

GenerationState drft::StructuresLayerChunk::generate(GenerationLevel)
{
    if (auto structure = _layer.tryGetStructureInVolume(_volume))
    {
        const int depth = _volume.min.z / _volume.dimensions().z;
        GenerationContext context = { getGlobalSeed(), _layer.getRegistries() };
        auto structureLayer = structure->getOrGenerateLayer(depth, context);
        if (!structureLayer) return GenerationState::Complete;

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

            IPositionGenerator::PositionSet possiblePositions;
            // TODO: Area is temporary... hopefully
            PositionGeneratorContext positionGeneratorContext = { .seed = getLayerManager().getSeed(), .area = drft::spatial::BoundlessPositiveRect };

            if (obj.value.HasMember("conditions"))
            {
                for (auto&& condition : obj.value["conditions"].GetArray())
                {
                    if (condition.HasMember("type"))
                    {
                        const std::string conditionType = condition["type"].GetString();
                        auto positionGenerator = getRegistries().positionGenerators.build(conditionType);
                        if (!positionGenerator)
                        {
                            LOG_ERROR("Structure {} contains a 'condition' 'type' with invalid name {}", structureName, conditionType);
                            continue;
                        }
                        positionGenerator->createFromJson(condition["params"]);
                        auto positions = positionGenerator->generatePositions(positionGeneratorContext);
                        possiblePositions.insert(positions.begin(), positions.end());
                    }
                    else
                    {
                        LOG_ERROR("Structure {} is missing has a 'condition' that is missing a 'type'", structureName);
                    }
                }
            }

            // TODO: Add filtering so not all positions are selected
            for (auto&& position : possiblePositions)
            {
                _structures.emplace_back(structureTemplate, position);
            }
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
        if (volume.intersects(structure.getVolume())) 
            return &structure;
    }
    return nullptr;
}
