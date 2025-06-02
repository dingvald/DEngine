#include "pch.h"
#include "Conversions.h"

using namespace drft::spatial;

namespace Internal
{
    static constexpr float constexprFloor(float f)
    {
        return (f >= 0.0f || static_cast<int>(f) == f) ? static_cast<int>(f) : static_cast<int>(f) - 1;
    }
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

drft::ChunkPosition drft::spatial::toChunkSpace(TilePosition tilePosition)
{
    ChunkPosition result;
    result.x = (tilePosition.x >= 0) ? (tilePosition.x / ChunkDimensions.x)
                                  : ((tilePosition.x + 1) / ChunkDimensions.x - 1);
    result.y = (tilePosition.y >= 0) ? (tilePosition.y / ChunkDimensions.y)
                                  : ((tilePosition.y + 1) / ChunkDimensions.y - 1);
    result.z = (tilePosition.z >= 0) ? (tilePosition.z / ChunkDimensions.z)
                                  : ((tilePosition.z + 1) / ChunkDimensions.z - 1);
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
    result.x = (position.x >= 0) ? (static_cast<int>(std::floor(position.x)) / TileDimensions.x)
                              : ((static_cast<int>(std::floor(position.x)) + 1) / TileDimensions.x - 1);
    result.y = (position.y >= 0) ? (static_cast<int>(std::floor(position.y)) / TileDimensions.y)
                              : ((static_cast<int>(std::floor(position.y)) + 1) / TileDimensions.y - 1);
    result.z = (position.z >= 0) ? (static_cast<int>(std::floor(position.z)) / TileDimensions.z)
                              : ((static_cast<int>(std::floor(position.z)) + 1) / TileDimensions.z - 1);
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
