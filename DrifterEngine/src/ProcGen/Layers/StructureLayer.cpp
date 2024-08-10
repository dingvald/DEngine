#include "pch.h"
#include "StructureLayer.h"

GenerationState details::StructureLayerChunk::generate()
{
    return GenerationState::Complete;
}

void details::StructureLayerChunk::destroy()
{
}

StructureLayer::StructureLayer()
{
    setChunkDimensions({ 64, 64 });
}
