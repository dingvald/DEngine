#pragma once
#include <vector>
#include <unordered_map>

namespace util
{
	template<typename T, typename U>
	std::vector<T> extractKeys(const std::unordered_map<T, U>& map)
	{
		std::vector<T> result(map.size());
		for (auto&& [key, _] : map)
		{
			result.push_back(key);
		}
		return result;
	}
}