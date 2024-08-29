#pragma once
#include <ProcGen/LayeredProcGen/LayeredProcGen.h>

class StructureLayer;

class StructureLayerChunk : public GenerationChunk<StructureLayer, StructureLayerChunk>
{
public:
	using GenerationChunk::GenerationChunk;

	virtual GenerationState generate(int level) override;
};



class StructureLayer : public GenerationLayer<StructureLayer, StructureLayerChunk>
{
public:
	StructureLayer();
};