#pragma once

#include <ProcGen/LayeredProcGen/LayeredProcGen.h>

class LandLayer;

class LandLayerChunk : public GenerationChunk<LandLayer, LandLayerChunk>
{
public:
	using GenerationChunk::GenerationChunk;
	virtual GenerationState generate() override;

	entt::dense_set<sf::Vector2i> landPoints;
	entt::dense_set<sf::Vector2i> oceanPoints;
};

class LandLayer : public GenerationLayer<LandLayer, LandLayerChunk>
{
public:
	LandLayer();

	std::vector<sf::Vector2i> getLandPointsInBounds(sf::IntRect area);
	std::vector<sf::Vector2i> getOceanPointsInBounds(sf::IntRect area);

	entt::id_type getInputLayerID() const;

private:
	entt::id_type _inputLayerID = {};
};
