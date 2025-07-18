#pragma once
#include <ProcGen/LayeredProcGen/LayeredProcGen.h>
#include <JSON/ICreateFromJson.h>
#include <Structures/Instance/StructureInstance.h>

namespace drft
{
    class StructuresLayer;

    class StructuresLayerChunk : public GenerationChunk<StructuresLayer, StructuresLayerChunk>
    {
    public:
        using GenerationChunk::GenerationChunk;
        virtual GenerationState generate(GenerationLevel desiredLevel) override;

        SlotPositionMap slots;
    };

    class StructuresLayer : public GenerationLayer<StructuresLayer, StructuresLayerChunk>, public ICreateFromJson
    {
    public:
        using GenerationLayer::GenerationLayer;

        void createFromJson(const rapidjson::Value& json) override;
        sf::Vector3i getChunkDimensions() const override;
        StructureInstance* tryGetStructureInVolume(const spatial::AABBi& volume);

    private:
        std::vector<StructureInstance> _structures;
    };
}
