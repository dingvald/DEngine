#include "pch.h"
#include "RandomSpread.h"
#include "Random/RandomNumberGenerator.h"

std::vector<sf::Vector2i> drft::gen::randomSpread(int, const spatial::Grid<int>& grid, GenerationParameters params)
{
	int number = static_cast<int>(params.at("Number"));

	std::vector<sf::Vector2i> result;
	result.reserve(number);
	for (int i = 0; i < number; ++i)
	{
		int x = rng::RandomNumberGenerator::intInRange(0, grid.width() - 1);
		int y = rng::RandomNumberGenerator::intInRange(0, grid.height() - 1);
		result.emplace_back(x, y);
	}
	return result;
}
