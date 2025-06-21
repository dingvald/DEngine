#pragma once

#include <ProcGen/LayeredProcGen/LayeredProcGen.h>

namespace drft
{
	class FeatureLayer;

	class FeatureLayerChunk : public GenerationChunk<FeatureLayer, FeatureLayerChunk>
	{
	public:
		using GenerationChunk::GenerationChunk;
		virtual GenerationState generate(GenerationLevel desiredLevel) override;
		
		std::vector<GeneratedFeature> generatedFeatures;
	private:
		GenerationState generateFeatures();
		GenerationState placeFeatures();
		virtual GenerationLevel numLevels() const override { return GenerationLevel::Two; }
	};

	class FeatureLayer : public GenerationLayer<FeatureLayer, FeatureLayerChunk>
	{
	public:
		using GenerationLayer::GenerationLayer;
		
		sf::Vector3i getChunkDimensions() const override;
	};
}