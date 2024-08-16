#pragma once

#include <ProcGen/LayeredProcGen/LayeredProcGen.h>

class JitteredGridLayer;

class JitteredGridLayerChunk : public GenerationChunk<JitteredGridLayer, JitteredGridLayerChunk>
{
public:
	using GenerationChunk::GenerationChunk;
	virtual GenerationState generate() override;
	sf::Vector2i jitteredPoint;
};

class JitteredGridLayer : public GenerationLayer<JitteredGridLayer, JitteredGridLayerChunk>
{
public:
	JitteredGridLayer();

	std::vector<sf::Vector2i> getPointsInBounds(sf::IntRect area);
};