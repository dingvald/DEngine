#pragma once
#include <Spatial/TilePosition.h>
#include <Spatial/ChunkPosition.h>

namespace drft::spatial
{
	// Spatial conversion functions //

	// Converts a position in world space into a position in tile space.
	sf::Vector2i toTileSpace(sf::Vector2f worldPosition);

	// Converts a chunk coordinate into a position in tile space.
	sf::Vector2i toTileSpace(sf::Vector2i chunkCoordinate);

	// Converts a position in tile space into a position in float space.
	sf::Vector2f toFloatSpace(sf::Vector2i tilePosition);

	// Converts a tile position into a chunk coordinate
	sf::Vector2i toChunkCoordinate(sf::Vector2i tilePosition);

	// Converts a world position into a chunk coordinate
	sf::Vector2i toChunkCoordinate(sf::Vector2f worldPosition);

	// Converts a tile position into a chunk-relative position.
	// Upper-left corner of chunk is (0,0)
	sf::Vector2i toLocalChunkSpace(sf::Vector2i tilePosition);

	TilePosition asTileSpace(sf::Vector2i position);
	TilePosition asTileSpace(sf::Vector3i position);
	ChunkPosition asChunkSpace(sf::Vector3i position);

	ChunkPosition toChunkSpace(sf::Vector3f position);
	ChunkPosition toChunkSpace(TilePosition tilePosition);
	sf::Vector3i toChunkLocalSpace(TilePosition tilePosition);
	TilePosition toTileSpace(ChunkPosition chunkPosition);
	TilePosition toTileSpace(sf::Vector3f position);
	sf::Vector3f toFloatSpace(TilePosition tilePosition);
	sf::Vector3f toFloatSpace(ChunkPosition chunkPosition);
}