#include "pch.h"
#include "StructureLayer.h"

GenerationState StructureLayerChunk::generate()
{
    return GenerationState::Complete;
}

StructureLayer::StructureLayer()
    : GenerationLayer({64, 64})
{
}
