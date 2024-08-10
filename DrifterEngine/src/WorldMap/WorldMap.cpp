#include "pch.h"
#include "WorldMap.h"
#include "ProcGen/WorldGeneration/WorldGenerator.h"
#include "Spatial/Conversions.h"

drft::WorldMap::WorldMap(const gen::WorldGenerator& generator)
	: _generator(generator)
{
	
}

sf::Vector2i drft::WorldMap::getDimensions() const
{
	WorldMapPosition worldMapSpace;
	worldMapSpace.fromChunkSpace(_generator.getDimensions());
	return { worldMapSpace.x, worldMapSpace.y };
}

const Biome* drft::WorldMap::getBiome(WorldMapPosition position) const
{
	return nullptr;
}
