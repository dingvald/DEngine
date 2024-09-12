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

drft::WorldMapPosition drft::spatial::toWorldMapSpace(TilePosition tilePosition)
{
	WorldMapPosition result;
	result.x = tilePosition.x * WorldMapPosition::TileDimensions.x;
	result.y = tilePosition.y * WorldMapPosition::TileDimensions.y;
	return std::move(result);
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

drft::ChunkPosition drft::spatial::toChunkSpace(WorldMapPosition worldMapPosition)
{
	ChunkPosition result;
	result.x = worldMapPosition.x * WorldMapPosition::TileDimensions.x / ChunkDimensions.x;
	result.y = worldMapPosition.y * WorldMapPosition::TileDimensions.y / ChunkDimensions.y;
	result.z = 0;
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
