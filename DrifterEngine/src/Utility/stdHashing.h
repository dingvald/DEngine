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

template<>
struct std::hash<std::pair<int, int>>
{
	size_t operator() (const std::pair<int, int>& pos) const noexcept
	{
		size_t combine = static_cast<size_t>(pos.first);
		combine ^= static_cast<size_t>(pos.second) + 0x9e3779b9 + (static_cast<size_t>(pos.first) << 6) + (static_cast<size_t>(pos.second) >> 2);
		return std::hash<size_t>()(combine);
	}
};