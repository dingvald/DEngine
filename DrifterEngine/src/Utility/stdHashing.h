#pragma once
template<>
struct std::hash<sf::Vector2i>
{
	size_t operator() (const sf::Vector2i& pos) const noexcept
	{
		size_t combine = static_cast<size_t>(pos.x);
		combine ^= static_cast<size_t>(pos.y) + 0x9e3779b9 + (static_cast<size_t>(pos.x) << 6) + (static_cast<size_t>(pos.y) >> 2);
		return std::hash<size_t>()(combine);
	}
};