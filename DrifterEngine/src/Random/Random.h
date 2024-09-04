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
		std::optional<size_t> weightedSelection(const std::vector<std::pair<T, int>>& weightedElements)
		{
			if (weightedElements.size() == 0) return std::nullopt;
			if (weightedElements.size() == 1) return 0;

			int totalWeight = 0;
			for (auto&& [_, weight] : weightedElements)
			{
				totalWeight += weight;
			}
			int selection = intInRange(1, totalWeight);
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

	private:
		XORshift _gen;
	};
}



