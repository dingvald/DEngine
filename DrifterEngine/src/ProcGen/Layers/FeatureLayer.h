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
		
		std::optional<FeatureGenerationResult> optionalGeneratedFeature;
	private:
		GenerationState selectFeature();
		GenerationState checkNeighbors();
		GenerationState placeFeature();
		virtual GenerationLevel numLevels() const override { return GenerationLevel::Three; }
	};

	class FeatureLayer : public GenerationLayer<FeatureLayer, FeatureLayerChunk>
	{
	public:
		using GenerationLayer::GenerationLayer;
		
		sf::Vector3i getChunkDimensions() const override;
	};
}