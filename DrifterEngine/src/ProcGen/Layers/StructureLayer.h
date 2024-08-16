#pragma once
#include <ProcGen/LayeredProcGen/LayeredProcGen.h>

class StructureLayer;

class StructureLayerChunk : public GenerationChunk<StructureLayer, StructureLayerChunk>
{
public:
	using GenerationChunk::GenerationChunk;

	virtual GenerationState generate() override;
};



class StructureLayer : public GenerationLayer<StructureLayer, StructureLayerChunk>
{
public:
	StructureLayer();
};