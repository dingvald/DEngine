#pragma once

#include <ProcGen/LayeredProcGen/LayeredProcGen.h>


class VoronoiLayer;

class VoronoiLayerChunk : public GenerationChunk<VoronoiLayer, VoronoiLayerChunk>
{
public:
	using GenerationChunk::GenerationChunk;
	virtual GenerationState generate(int level) override;

	std::vector<sf::Vector2i> centroids;
	std::vector<std::pair<sf::Vector2i, sf::Vector2i>> edges;
};

class VoronoiLayer : public GenerationLayer<VoronoiLayer, VoronoiLayerChunk>
{
public:
	VoronoiLayer();

	std::vector<sf::Vector2i> getCentroidsInBounds(sf::IntRect area);
	std::vector<std::pair<sf::Vector2i, sf::Vector2i>> getEdgesInBounds(sf::IntRect area);
};