#pragma once
#include <Spatial/TilePosition.h>
#include <Spatial/ChunkPosition.h>

namespace drft::spatial
{
	// Spatial conversion functions //

	TilePosition asTileSpace(sf::Vector2i position);
	TilePosition asTileSpace(sf::Vector3i position);
	ChunkPosition asChunkSpace(sf::Vector3i position);

	ChunkPosition toChunkSpace(TilePosition tilePosition);
	sf::Vector3i toChunkLocalSpace(TilePosition tilePosition);
	TilePosition toTileSpace(ChunkPosition chunkPosition);
	TilePosition toTileSpace(sf::Vector3f position);
	sf::Vector3f toFloatSpace(TilePosition tilePosition);
	sf::Vector3f toFloatSpace(ChunkPosition chunkPosition);
}