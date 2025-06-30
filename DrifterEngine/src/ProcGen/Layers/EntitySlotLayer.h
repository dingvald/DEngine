#pragma once

#include <ProcGen/LayeredProcGen/LayeredProcGen.h>
#include <ProcGen/SlotPositionList.h>

namespace drft
{
	class EntitySlotLayer;

	class EntitySlotLayerChunk : public GenerationChunk<EntitySlotLayer, EntitySlotLayerChunk>
	{
	public:
		using GenerationChunk::GenerationChunk;
		virtual GenerationState generate(GenerationLevel desiredLevel) override;
		SlotPositionMap slots;
	};

	class EntitySlotLayer : public GenerationLayer<EntitySlotLayer, EntitySlotLayerChunk>
	{
	public:
		using GenerationLayer::GenerationLayer;

		sf::Vector3i getChunkDimensions() const override;
	};
}