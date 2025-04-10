#pragma once

#include <ProcGen/LayeredProcGen/LayeredProcGen.h>


namespace drft
{
	class JitteredGridLayer;

	class JitteredGridLayerChunk : public GenerationChunk<JitteredGridLayer, JitteredGridLayerChunk>
	{
	public:
		using GenerationChunk::GenerationChunk;
		virtual GenerationState generate(GenerationLevel desiredLevel) override;
		sf::Vector2i jitteredPoint;
	};

	class JitteredGridLayer : public GenerationLayer<JitteredGridLayer, JitteredGridLayerChunk>
	{
	public:
		using GenerationLayer::GenerationLayer;

		std::vector<sf::Vector2i> getPointsInArea(sf::IntRect area, sf::Vector3i origin);
		sf::Vector3i getChunkDimensions() const override;
	};
}
