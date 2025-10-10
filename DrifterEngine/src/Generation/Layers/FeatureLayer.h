#pragma once

#include <Generation/LayeredProcGen/LayeredProcGen.h>
#include <Generation/SlotPositionList.h>

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
		void placeFeature(const SlotPositionList& slots);
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