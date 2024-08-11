#include "pch.h"
#include "EntityLayer.h"
#include <ProcGen/Layers/BiomeLayer.h>
#include <ProcGen/Layers/StructureLayer.h>


namespace details
{
	GenerationState EntityLayerChunk::generate()
	{

		return GenerationState::Complete;
	}

	void EntityLayerChunk::destroy()
	{

	}
}


EntityLayer::EntityLayer()
{
	setChunkDimensions({ 8, 8 });
	addDependency<BiomeLayer>({ 2, 2 });
	addDependency<StructureLayer>({ 2,2 });
}

EntityMap EntityLayer::getEntitiesInBounds(sf::IntRect area)
{
	EntityMap result;
	forEachLoadedChunkInArea(area, [area, &result](details::EntityLayerChunk& chunk)
		{
			for (auto&& [position, entities] : chunk.entities)
			{
				if (!area.contains(position)) continue;
				result.emplace(position, std::vector{ entities });
			}
		});
	return result;
}
