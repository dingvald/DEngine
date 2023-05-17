#pragma once

namespace drft::spatial
{
	// Spatial conversion functions //

	const int CHUNK_WIDTH = 64;
	const int CHUNK_HEIGHT = 64;
	const int TILE_WIDTH = 16;
	const int TILE_HEIGHT = 16;


	// Converts a position in world space into a position in tile space.
	sf::Vector2i toTileSpace(const sf::Vector2f worldPosition);

	// Converts a chunk coordinate into a position in tile space.
	sf::Vector2i toTileSpace(const sf::Vector2i chunkCoordinate);

	// Converts a position in tile space into a position in world space.
	sf::Vector2f toWorldSpace(const sf::Vector2i tilePosition);

	// Converts a tile position into a chunk coordinate
	sf::Vector2i toChunkCoordinate(const sf::Vector2i tilePosition);

	// Converts a world position into a chunk coordinate
	sf::Vector2i toChunkCoordinate(const sf::Vector2f worldPosition);

	// Converts a tile position into a chunk-relative position.
	// Upper-left corner of chunk is (0,0)
	sf::Vector2i toLocalChunkSpace(const sf::Vector2i tilePosition);
}

