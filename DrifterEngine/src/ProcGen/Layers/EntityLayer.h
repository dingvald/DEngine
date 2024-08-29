#pragma once
#include <ProcGen/LayeredProcGen/LayeredProcGen.h>

class EntityLayer;

using EntityMap = entt::dense_map<sf::Vector2i, std::vector<std::string>>;


class EntityLayerChunk : public GenerationChunk<EntityLayer, EntityLayerChunk>
{
public:
	using GenerationChunk::GenerationChunk;

	virtual GenerationState generate(int level) override;

	EntityMap entities;
};

class EntityLayer : public GenerationLayer<EntityLayer, EntityLayerChunk>
{
public:
	EntityLayer();

	EntityMap getEntitiesInBounds(sf::IntRect area);
};

