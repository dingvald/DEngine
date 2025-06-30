#pragma once

#include <ProcGen/LayeredProcGen/LayeredProcGen.h>
#include <ProcGen/SlotPositionList.h>

namespace drft
{
	class FeatureLayer;

	class FeatureLayerChunk : public GenerationChunk<FeatureLayer, FeatureLayerChunk>
	{
	public:
		using GenerationChunk::GenerationChunk;
		virtual GenerationState generate(GenerationLevel desiredLevel) override;
		
		std::vector<GeneratedFeature> generatedFeatures;
		SlotPositionMap slots;
	private:
		GenerationState generateFeatures();
		GenerationState generateSurroundingFeatures();
		virtual GenerationLevel numLevels() const override { return GenerationLevel::Two; }
	};

	class FeatureLayer : public GenerationLayer<FeatureLayer, FeatureLayerChunk>
	{
	public:
		using GenerationLayer::GenerationLayer;

		sf::Vector3i getChunkDimensions() const override;
	};
}