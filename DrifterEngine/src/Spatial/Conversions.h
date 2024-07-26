#pragma once

namespace drft::spatial
{
	// Spatial conversion functions //

	constexpr int CHUNK_WIDTH = 8;
	constexpr int CHUNK_HEIGHT = 8;
	constexpr int TILE_WIDTH = 16;
	constexpr int TILE_HEIGHT = 16;

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
}