#pragma once
#include <Utility/stdHashing.h>
#include <Spatial/TilePosition.h>

namespace drft
{
	// Dimensions in Tiles
	static const sf::Vector3i ChunkDimensions = { 8, 8, 8 };

	struct ChunkPosition : public sf::Vector3i
	{
		using sf::Vector3i::Vector3;

		static std::string toString(const ChunkPosition& chunkPosition);
	};
}

namespace cereal
{
	template<typename Archive>
	void serialize(Archive& archive, drft::ChunkPosition& pos)
	{
		archive(pos.x, pos.y, pos.z);
	}
}

template<>
struct std::hash<drft::ChunkPosition>
{
	size_t operator() (const drft::ChunkPosition& pos) const noexcept
	{
		return std::hash<sf::Vector3i>()(pos);
	}
};