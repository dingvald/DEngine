#include "pch.h"
#include "ChunkPosition.h"

std::string drft::ChunkPosition::toString(const ChunkPosition& chunkPosition)
{
    return "(" + std::to_string(chunkPosition.x) + ", " + std::to_string(chunkPosition.y) + ", " + std::to_string(chunkPosition.z) + ")";
}
