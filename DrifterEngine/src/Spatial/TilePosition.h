#pragma once
#include <Utility/stdHashing.h>
#include <Spatial/ChunkPosition.h>

namespace drft
{
	// Dimensions in ints
	static const sf::Vector3i TileDimensions = { 16, 24, 16 };

	struct TilePosition : public sf::Vector3i
	{
		using sf::Vector3i::Vector3;
		explicit TilePosition(sf::Vector3i position);

		static std::string toString(const TilePosition& tilePosition);
	};

	TilePosition operator -(const TilePosition& left);
	TilePosition operator +=(TilePosition& left, const TilePosition& right);
	TilePosition operator -=(TilePosition& left, const TilePosition& right);
	TilePosition operator +(const TilePosition& left, const TilePosition& right);
	TilePosition operator -(const TilePosition& left, const TilePosition& right);
}

namespace cereal
{
	template<typename Archive>
	void serialize(Archive& archive, drft::TilePosition& pos)
	{
		archive(pos.x, pos.y, pos.z);
	}
}

template<>
struct std::hash<drft::TilePosition>
{
	size_t operator() (const drft::TilePosition& pos) const noexcept
	{
		return std::hash<sf::Vector3i>()(pos);
	}
};

