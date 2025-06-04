#pragma once

#include <ProcGen/LayeredProcGen/LayeredProcGen.h>
#include <ProcGen/EntityPositionList.h>

namespace drft
{
	class TilePlacementLayer;

	class TilePlacementLayerChunk : public GenerationChunk<TilePlacementLayer, TilePlacementLayerChunk>
	{
	public:
		using GenerationChunk::GenerationChunk;
		virtual GenerationState generate(GenerationLevel desiredLevel) override;
		void placeTiles(entt::registry& registry) const;

	private:
		std::vector<std::pair<sf::Vector3i, sf::Color>> _tiles;
	};

	class TilePlacementLayer : public GenerationLayer<TilePlacementLayer, TilePlacementLayerChunk>
	{
	public:
		using GenerationLayer::GenerationLayer;

		void placeTiles(spatial::AABB<int> volume, entt::registry& registry);

		sf::Vector3i getChunkDimensions() const override;
	};
}