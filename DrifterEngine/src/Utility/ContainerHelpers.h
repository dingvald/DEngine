#pragma once
#include <vector>
#include <unordered_map>

namespace drft::util
{
	template<typename T, typename U>
	std::vector<T> extractKeys(const std::unordered_map<T, U>& map)
	{
		std::vector<T> result;
		result.reserve(map.size());
		for (auto&& [key, _] : map)
		{
			result.push_back(key);
		}
		return result;
	}

	template<typename T>
	concept Hashable = requires(const T & t) {
		{ std::hash<T>{}(t) } -> std::same_as<std::size_t>;
	};

	template<Hashable T>
	std::vector<T> vector_union(const std::vector<T>& v1, const std::vector<T>& v2)
	{
		std::unordered_set<T> added;
		std::vector<T> result;

		for (auto&& val : v1) {
			if (!added.contains(val)) {
				result.push_back(val);
				added.insert(val);
			}
		}

		for (auto&& val : v2) {
			if (!added.count(val)) {
				result.push_back(val);
				added.insert(val);
			}
		}

		return result;
	}

	template<Hashable T>
	std::vector<T> vector_intersection(const std::vector<T>& v1, const std::vector<T>& v2)
	{
		std::unordered_set<T> set1(v1.begin(), v1.end());
		std::unordered_set<T> added;
		std::vector<T> result;
		result.reserve(std::min(v1.size(), v2.size()));

		for (auto&& val : v2) {
			if (set1.contains(val) && !added.contains(val)) {
				result.push_back(val);
				added.insert(val);
			}
		}

		return result;
	}
}