#include "pch.h"
#include "WorldMap.h"
#include "ProcGen/WorldGeneration/WorldGenerator.h"
#include "Spatial/Conversions.h"

#pragma optimize("", off)

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
	WorldMapPosition oneMapTile = { 1, 1 };
	auto halfMapTileInTileSpace = oneMapTile.toTileSpace();
	// Create a half world tile in tile space
	halfMapTileInTileSpace.x /= 2;
	halfMapTileInTileSpace.y /= 2;

	const Biome* biome = _generator.getBiome(position.toChunkSpace() + spatial::toChunkCoordinate(halfMapTileInTileSpace));
	return biome;
}
