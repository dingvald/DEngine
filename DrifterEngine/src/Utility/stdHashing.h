#pragma once
#include <memory> // For std::hash definition


template <typename T>
inline void hash_combine(std::size_t& seed, T const& v)
{
	std::hash<T> hasher;
	seed ^= hasher(v) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
}


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
struct std::hash<sf::Vector3i>
{
	size_t operator() (const sf::Vector3i& pos) const noexcept
	{
		std::hash<int> hasher;
		size_t seed = hasher(pos.x);
		hash_combine(seed, pos.y);
		hash_combine(seed, pos.z);
		return seed;
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

template<>
struct std::hash<entt::hashed_string>
{
	size_t operator() (const entt::hashed_string& str) const noexcept
	{
		return std::hash<entt::id_type>{}(str.value());
	}
};

