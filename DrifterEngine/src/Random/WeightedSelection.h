#pragma once
#include <vector>
#include <random>
#include "RandomNumberGenerator.h"

namespace drft::rng
{
	template<typename T>
	std::optional<size_t> weightedSelection(const std::vector<std::pair<T, int>>& weightedElements)
	{
		if (weightedElements.size() == 0) return std::nullopt;
		if (weightedElements.size() == 1) return 0;

		int totalWeight = 0;
		for (auto&& [_, weight] : weightedElements)
		{
			totalWeight += weight;
		}
		int selection = rng::RandomNumberGenerator::intInRange(1, totalWeight);
		int cursor = 0;
		size_t index = 0;
		for (auto&& [_, weight] : weightedElements)
		{
			cursor += weight;
			if (cursor >= selection)
			{
				return index;
			}
			index++;
		}
		return std::nullopt;
	}
}