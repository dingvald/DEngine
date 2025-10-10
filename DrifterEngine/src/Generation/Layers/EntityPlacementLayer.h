#pragma once

#include <Generation/LayeredProcGen/LayeredProcGen.h>
#include <Generation/EntityPositionList.h>

class EntityPack;

namespace drft
{
	class EntityFactory;

	class EntityPlacementLayer;

	class EntityPlacementLayerChunk : public GenerationChunk<EntityPlacementLayer, EntityPlacementLayerChunk>
	{
	public:
		using GenerationChunk::GenerationChunk;
		virtual GenerationState generate(GenerationLevel desiredLevel) override;
		virtual GenerationLevel numLevels() const override { return GenerationLevel::Two; }

	public:
		EntityPositionMap chosenEntities;

	private:
		GenerationState chooseEntitiesForSlots();
		GenerationState resolveWithNeighborChunks();
	};

	class EntityPlacementLayer : public GenerationLayer<EntityPlacementLayer, EntityPlacementLayerChunk>
	{
	public:
		using GenerationLayer::GenerationLayer;

		void placeEntities(spatial::AABB<int> volume, entt::registry& registry);

		sf::Vector3i getChunkDimensions() const override;
	};
}