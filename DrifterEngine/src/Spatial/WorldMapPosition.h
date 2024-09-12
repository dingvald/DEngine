#pragma once
#include <SFML/System/Vector2.hpp>

namespace drft
{
	struct WorldMapPosition
	{
		static inline const sf::Vector2i TileDimensions = { 64, 64 };

		WorldMapPosition() = default;
		WorldMapPosition(int x, int y);

		int x = 0;
		int y = 0;

		WorldMapPosition operator+(sf::Vector2i rhs) const;
		WorldMapPosition operator-(sf::Vector2i rhs) const;
		void operator+=(sf::Vector2i rhs);
		void operator-=(sf::Vector2i rhs);
		bool operator==(const WorldMapPosition& other) const;
	};
}

namespace cereal
{
	template<typename Archive>
	void serialize(Archive& archive, drft::WorldMapPosition& pos)
	{
		archive(pos.x, pos.y);
	}
}

template<>
struct std::hash<drft::WorldMapPosition>
{
	size_t operator() (const drft::WorldMapPosition& pos) const noexcept
	{
		size_t combine = static_cast<size_t>(pos.x);
		combine ^= static_cast<size_t>(pos.y) + 0x9e3779b9 + (static_cast<size_t>(pos.x) << 6) + (static_cast<size_t>(pos.y) >> 2);
		return std::hash<size_t>()(combine);
	}
};