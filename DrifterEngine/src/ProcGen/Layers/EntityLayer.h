#pragma once
#include <ProcGen/LayeredProcGen/LayeredProcGen.h>

class EntityLayer;

using EntityMap = entt::dense_map<sf::Vector2i, std::vector<std::string>>;

namespace details
{
	class EntityLayerChunk : public GenerationChunk<EntityLayer, EntityLayerChunk>
	{
	public:
		using GenerationChunk::GenerationChunk;

		virtual GenerationState generate() override;
		virtual void destroy() override;

		EntityMap entities;
	};
}

class EntityLayer : public GenerationLayer<EntityLayer, details::EntityLayerChunk>
{
public:
	EntityLayer();
	EntityMap getEntitiesInBounds(sf::IntRect area);

private:

};

