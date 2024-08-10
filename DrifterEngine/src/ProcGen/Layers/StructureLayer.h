#pragma once
#include <ProcGen/LayeredProcGen/LayeredProcGen.h>

class StructureLayer;

namespace details
{
	class StructureLayerChunk : public GenerationChunk<StructureLayer, StructureLayerChunk>
	{
	public:
		using GenerationChunk::GenerationChunk;

		virtual GenerationState generate() override;
		virtual void destroy() override;
	};
}


class StructureLayer : public GenerationLayer<StructureLayer, details::StructureLayerChunk>
{
public:
	StructureLayer();
	
};