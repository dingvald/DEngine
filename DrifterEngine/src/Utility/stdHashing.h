#pragma once
#include <memory> // For std::hash definition
#include <Spatial/AABB.h>


template<typename T>
concept Hashable = requires(T a) {
    { std::hash<T>{}(a) } -> std::convertible_to<std::size_t>;
};

namespace drft
{
	template<Hashable T>
	size_t hash(const T& val)
	{
		return std::hash<T>{}(val);
	}
}


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

template<>
struct std::hash<drft::spatial::AABBi>
{
	std::size_t operator()(const drft::spatial::AABBi& aabb) const
	{
		std::size_t h1 = std::hash<sf::Vector3i>{}(aabb.min);
		std::size_t h2 = std::hash<sf::Vector3i>{}(aabb.max);
		hash_combine(h1, h2);
		return h1;
	}
};

