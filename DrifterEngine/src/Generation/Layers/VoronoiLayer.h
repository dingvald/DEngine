#pragma once

#include <Generation/LayeredProcGen/LayeredProcGen.h>

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
		std::vector<sf::Vector3i> getCentroidsInArea(sf::IntRect area, int z);
		std::vector<std::pair<sf::Vector3i, sf::Vector3i>> getEdgesInArea(sf::IntRect area, int z);

		sf::Vector3i getChunkDimensions() const override;
	};
}
