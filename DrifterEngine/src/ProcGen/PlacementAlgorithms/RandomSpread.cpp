#include "pch.h"
#include "RandomSpread.h"
#include "Random/RandomNumberGenerator.h"

std::vector<sf::Vector2i> drft::gen::randomSpread(const spatial::Grid<int>& grid, GenerationParameters params, int seed)
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
		do {
			x = rng::RandomNumberGenerator::intInRange(0, grid.width() - 1);
			y = rng::RandomNumberGenerator::intInRange(0, grid.height() - 1);
		} 
		while (grid.at(x, y)  != 0);
		
		result.emplace_back(x, y);
	}
	return result;
}
