#include "pch.h"
#include "Conversions.h"

using namespace drft::spatial;

sf::Vector2i drft::spatial::toTileSpace(sf::Vector2f worldPosition)
{
	int x = static_cast<int>(std::floor(worldPosition.x / TileDimensions.x));
	int y = static_cast<int>(std::floor(worldPosition.y / TileDimensions.y));

	return { x, y };
}

sf::Vector2i drft::spatial::toTileSpace(sf::Vector2i chunkCoordinate)
{
	int x = chunkCoordinate.x * ChunkDimensions.x;
	int y = chunkCoordinate.y * ChunkDimensions.y;

	return { x,y };
}

sf::Vector2f drft::spatial::toFloatSpace(sf::Vector2i tilePosition)
{
	float x = static_cast<float>(tilePosition.x * TileDimensions.x);
	float y = static_cast<float>(tilePosition.y * TileDimensions.y);

	return { x, y };
}

sf::Vector2i drft::spatial::toChunkCoordinate(sf::Vector2i tilePosition)
{
	int xChunk = tilePosition.x / ChunkDimensions.x;
	if (tilePosition.x < 0)
	{
		xChunk = (tilePosition.x + 1) / ChunkDimensions.x;
		--xChunk;
	}

	int yChunk = tilePosition.y / ChunkDimensions.y;
	if (tilePosition.y < 0)
	{
		yChunk = (tilePosition.y + 1) / ChunkDimensions.y;
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
	int xPos = tilePosition.x % ChunkDimensions.x;
	if (xPos < 0)
	{
		xPos += ChunkDimensions.x;
	}

	int yPos = tilePosition.y % ChunkDimensions.y;
	if (yPos < 0)
	{
		yPos += ChunkDimensions.y;
	}

	return { xPos, yPos };
}

drft::TilePosition drft::spatial::asTileSpace(sf::Vector2i position)
{
	return TilePosition{ position.x, position.y, 0 };
}

drft::TilePosition drft::spatial::asTileSpace(sf::Vector3i position)
{
	TilePosition result = { position.x, position.y, position.z };
	return std::move(result);
}

drft::ChunkPosition drft::spatial::asChunkSpace(sf::Vector3i position)
{
	ChunkPosition result = { position.x, position.y, position.z };
	return std::move(result);
}

drft::ChunkPosition drft::spatial::toChunkSpace(sf::Vector3f position)
{
	ChunkPosition result;
	result.x = static_cast<int>(position.x) / TileDimensions.x / ChunkDimensions.x;
	result.y = static_cast<int>(position.y) / TileDimensions.y / ChunkDimensions.y;
	result.z = static_cast<int>(position.z) / TileDimensions.z / ChunkDimensions.z;
	return std::move(result);
}

drft::ChunkPosition drft::spatial::toChunkSpace(TilePosition tilePosition)
{
	ChunkPosition result;
	result.x = tilePosition.x / ChunkDimensions.x;
	result.y = tilePosition.y / ChunkDimensions.y;
	result.z = tilePosition.z / ChunkDimensions.z;
	return std::move(result);
}

sf::Vector3i drft::spatial::toChunkLocalSpace(TilePosition tilePosition)
{
	int xPos = tilePosition.x % ChunkDimensions.x;
	if (xPos < 0)
	{
		xPos += ChunkDimensions.x;
	}

	int yPos = tilePosition.y % ChunkDimensions.y;
	if (yPos < 0)
	{
		yPos += ChunkDimensions.y;
	}

	int zPos = tilePosition.z % ChunkDimensions.z;
	if (zPos < 0)
	{
		zPos += ChunkDimensions.z;
	}

	return { xPos, yPos, zPos };
}

drft::TilePosition drft::spatial::toTileSpace(ChunkPosition chunkPosition)
{
	TilePosition result;
	result.x = chunkPosition.x * ChunkDimensions.x;
	result.y = chunkPosition.y * ChunkDimensions.y;
	result.z = chunkPosition.z * ChunkDimensions.z;
	return std::move(result);
}

drft::TilePosition drft::spatial::toTileSpace(sf::Vector3f position)
{
	TilePosition result;
	result.x = static_cast<int>(position.x) / TileDimensions.x;
	result.y = static_cast<int>(position.y) / TileDimensions.y;
	result.z = static_cast<int>(position.z) / TileDimensions.z;
	return std::move(result);
}

sf::Vector3f drft::spatial::toFloatSpace(TilePosition tilePosition)
{
	sf::Vector3f result;
	result.x = static_cast<float>(tilePosition.x * TileDimensions.x);
	result.y = static_cast<float>(tilePosition.y * TileDimensions.y);
	result.z = static_cast<float>(tilePosition.z * TileDimensions.z);
	return std::move(result);
}

sf::Vector3f drft::spatial::toFloatSpace(ChunkPosition chunkPosition)
{
	sf::Vector3f result;
	result.x = static_cast<float>(chunkPosition.x * TileDimensions.x * ChunkDimensions.x);
	result.y = static_cast<float>(chunkPosition.y * TileDimensions.y * ChunkDimensions.y);
	result.z = static_cast<float>(chunkPosition.z * TileDimensions.z * ChunkDimensions.z);
	return std::move(result);
}
