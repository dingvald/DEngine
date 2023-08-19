#include "pch.h"
#include "Conversions.h"

using namespace drft::spatial;

sf::Vector2i drft::spatial::toTileSpace(sf::Vector2f worldPosition)
{
	int x = static_cast<int>(std::floor(worldPosition.x / TILE_WIDTH));
	int y = static_cast<int>(std::floor(worldPosition.y / TILE_HEIGHT));

	return { x, y };
}

sf::Vector2i drft::spatial::toTileSpace(sf::Vector2i chunkCoordinate)
{
	int x = chunkCoordinate.x * CHUNK_WIDTH;
	int y = chunkCoordinate.y * CHUNK_HEIGHT;

	return { x,y };
}

sf::Vector2f drft::spatial::toFloatSpace(sf::Vector2i tilePosition)
{
	float x = static_cast<float>(tilePosition.x * TILE_WIDTH);
	float y = static_cast<float>(tilePosition.y * TILE_HEIGHT);

	return { x, y };
}

sf::Vector2i drft::spatial::toChunkCoordinate(sf::Vector2i tilePosition)
{
	int xChunk = tilePosition.x / CHUNK_WIDTH;
	if (tilePosition.x < 0)
	{
		xChunk = (tilePosition.x + 1) / CHUNK_WIDTH;
		--xChunk;
	}

	int yChunk = tilePosition.y / CHUNK_HEIGHT;
	if (tilePosition.y < 0)
	{
		yChunk = (tilePosition.y + 1) / CHUNK_HEIGHT;
		--yChunk;
	}
	return { xChunk, yChunk };
}

sf::Vector2i drft::spatial::toChunkCoordinate(sf::Vector2f worldPosition)
{
	auto tilePosition = toTileSpace(worldPosition);
	return toChunkCoordinate(tilePosition);
}

sf::Vector2i drft::spatial::toLocalChunkSpace(sf::Vector2i tilePosition)
{
	int xPos = tilePosition.x % CHUNK_WIDTH;
	if (xPos < 0)
	{
		xPos += CHUNK_WIDTH;
	}

	int yPos = tilePosition.y % CHUNK_HEIGHT;
	if (yPos < 0)
	{
		yPos += CHUNK_HEIGHT;
	}

	return { xPos, yPos };
}
