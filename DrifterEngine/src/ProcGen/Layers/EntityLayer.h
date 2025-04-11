#pragma once

#include <ProcGen/LayeredProcGen/LayeredProcGen.h>

namespace drft
{
	class EntityLayer;

	class EntityLayerChunk : public GenerationChunk<EntityLayer, EntityLayerChunk>
	{
	public:
		using GenerationChunk::GenerationChunk;
		virtual GenerationState generate(GenerationLevel desiredLevel) override;
	};

	class EntityLayer : public GenerationLayer<EntityLayer, EntityLayerChunk>
	{
	public:
		using GenerationLayer::GenerationLayer;

		sf::Vector3i getChunkDimensions() const override;
	};
}