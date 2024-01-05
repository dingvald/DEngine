#include "pch.h"
#include "RandomSpread.h"
#include "Random/RandomNumberGenerator.h"

std::vector<sf::Vector2i> drft::gen::randomSpread(sf::IntRect area, const spatial::AutoGrid<std::bitset<32>>& grid, GenerationParameters params, int)
{
	int minimum = static_cast<int>(params.at("Minimum"));
	int maximum = static_cast<int>(params.at("Maximum"));

	int number = rng::RandomNumberGenerator::intInRange(minimum, maximum);

	std::vector<sf::Vector2i> result;
	result.reserve(number);
	for (int i = 0; i < number; ++i)
	{
		int x = 0;
		int y = 0;
		int tries = 10;
		do {
			x = rng::RandomNumberGenerator::intInRange(0, area.width - 1);
			y = rng::RandomNumberGenerator::intInRange(0, area.height - 1);
			--tries;
		} 
		while (grid.at(area.left + x, area.top + y).any() && tries > 0);
		if (tries <= 0) continue;

		result.emplace_back(x, y);
	}
	return result;
}
