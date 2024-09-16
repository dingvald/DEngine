#pragma once

#include <ProcGen/LayeredProcGen/LayeredProcGen.h>


namespace drft
{
	class JitteredGridLayer;

	class JitteredGridLayerChunk : public GenerationChunk<JitteredGridLayer, JitteredGridLayerChunk>
	{
	public:
		using GenerationChunk::GenerationChunk;
		virtual GenerationState generate(int level) override;
		sf::Vector2i jitteredPoint;
	};

	class JitteredGridLayer : public GenerationLayer<JitteredGridLayer, JitteredGridLayerChunk>
	{
	public:
		JitteredGridLayer();

		std::vector<sf::Vector2i> getPointsInArea(sf::IntRect area, sf::Vector3i origin);
	};
}
