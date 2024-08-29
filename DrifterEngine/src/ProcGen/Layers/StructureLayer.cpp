#include "pch.h"
#include "StructureLayer.h"

GenerationState StructureLayerChunk::generate(int level)
{
    return GenerationState::Complete;
}

StructureLayer::StructureLayer()
    : GenerationLayer({64, 64})
{
}
