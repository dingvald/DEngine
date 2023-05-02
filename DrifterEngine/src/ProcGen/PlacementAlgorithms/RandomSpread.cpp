#include "pch.h"
#include "RandomSpread.h"
#include "Random/RandomNumberGenerator.h"

std::vector<sf::Vector2i> drft::gen::randomSpread(int, sf::Vector2i bounds, GenerationParameters params)
{
	int number = params.at("Number");

	std::vector<sf::Vector2i> result;
	result.reserve(number);
	for (int i = 0; i < number; ++i)
	{
		int x = rng::RandomNumberGenerator::intInRange(0, bounds.x - 1);
		int y = rng::RandomNumberGenerator::intInRange(0, bounds.y - 1);
		result.emplace_back(x, y);
	}
	return result;
}
