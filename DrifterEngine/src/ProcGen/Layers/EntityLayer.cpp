#include "pch.h"
#include "EntityLayer.h"
#include <ProcGen/Layers/BiomeLayer.h>
#include <ProcGen/Layers/StructureLayer.h>


GenerationState EntityLayerChunk::generate()
{
	auto biomeLayer = generateDependency<BiomeLayer>(bounds());
	auto structureLayer = generateDependency<StructureLayer>(bounds());

	const GenerationState state = combinedState({ biomeLayer.state, structureLayer.state });
	if (state != GenerationState::Complete) return state;

	
	
	return GenerationState::Complete;
}


EntityLayer::EntityLayer()
	: GenerationLayer({8, 8})
{}

EntityMap EntityLayer::getEntitiesInBounds(sf::IntRect area)
{
	EntityMap result;
	forEachLoadedChunkInArea(area, [area, &result](EntityLayerChunk& chunk)
		{
			for (auto&& [position, entities] : chunk.entities)
			{
				if (!area.contains(position)) continue;
				result.emplace(position, std::vector{ entities });
			}
		});
	return result;
}
