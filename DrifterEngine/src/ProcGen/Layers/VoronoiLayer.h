#pragma once

#include <ProcGen/LayeredProcGen/LayeredProcGen.h>

namespace drft
{
	class VoronoiLayer;

	class VoronoiLayerChunk : public GenerationChunk<VoronoiLayer, VoronoiLayerChunk>
	{
	public:
		using GenerationChunk::GenerationChunk;
		virtual GenerationState generate(GenerationLevel desiredLevel) override;

		std::vector<sf::Vector2i> centroids;
		std::vector<std::pair<sf::Vector2i, sf::Vector2i>> edges;
	};

	class VoronoiLayer : public GenerationLayer<VoronoiLayer, VoronoiLayerChunk>
	{
	public:
		using GenerationLayer::GenerationLayer;
		std::vector<sf::Vector3i> getCentroidsInArea(sf::IntRect area, sf::Vector3i origin);
		std::vector<std::pair<sf::Vector3i, sf::Vector3i>> getEdgesInArea(sf::IntRect area, sf::Vector3i origin);

		sf::Vector3i getChunkDimensions() const override;
	};
}
