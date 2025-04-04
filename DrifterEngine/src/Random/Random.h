#pragma once
#include <Random/XORshift.h>

namespace drft::rng
{
	unsigned int generateSeed();
	static unsigned int GlobalSeed;

	class Random
	{
	public:
		Random(unsigned int seed);

		double realInRange(double min, double max);
		int intInRange(int min, int max);
		sf::Vector2i positionInCircle(sf::Vector2i origin, int radius);
		sf::Vector2i positionInRect(sf::IntRect rect);
		std::vector<sf::Vector2i> shuffleRect(sf::IntRect rect);

		template<typename T>
		const T* weightedSelection(const std::vector<std::pair<T, int>>& weightedElements)
		{
			if (weightedElements.size() == 0) return nullptr;
			if (weightedElements.size() == 1) return &weightedElements[0];

			int totalWeight = 0;
			for (auto&& [_, weight] : weightedElements)
			{
				totalWeight += weight;
			}
			int selection = intInRange(1, totalWeight);
			int cursor = 0;
			for (auto&& [val, weight] : weightedElements)
			{
				cursor += weight;
				if (cursor >= selection)
				{
					return &val;
				}
			}
			return nullptr;
		}

		template<typename T>
		const T* weightedSelection(const std::unordered_map<T, int>& weightedElements)
		{
			if (weightedElements.size() == 0) return nullptr;
			if (weightedElements.size() == 1) return &(weightedElements.begin()->first);

			int totalWeight = 0;
			for (auto&& [_, weight] : weightedElements)
			{
				totalWeight += weight;
			}
			int selection = intInRange(1, totalWeight);
			int cursor = 0;
			for (auto&& it = weightedElements.begin(); it != weightedElements.end(); ++it)
			{
				cursor += it->second;
				if (cursor >= selection)
				{
					return &(it->first);
				}
			}

			return nullptr;
		}

	private:
		XORshift _gen;
	};
}



