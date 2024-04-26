#include "pch.h"
#include "RandomSpread.h"
#include "Random/RandomNumberGenerator.h"

std::vector<sf::Vector2i> drft::gen::randomSpread(const GenerationContext& ctx, const GenerationParameters& params)
{
	int minimum = (int)std::get<float>(params.at("Minimum"));
	int maximum = (int)std::get<float>(params.at("Maximum"));

	int number = rng::RandomNumberGenerator::intInRange(minimum, maximum);

	std::vector<sf::Vector2i> result;
	result.reserve(number);
	for (int i = 0; i < number; ++i)
	{
		int x = 0;
		int y = 0;
		int tries = 10;
		do {
			x = rng::RandomNumberGenerator::intInRange(0, ctx.area.width - 1);
			y = rng::RandomNumberGenerator::intInRange(0, ctx.area.height - 1);
			--tries;
		} 
		while (ctx.grid.at(ctx.area.left + x, ctx.area.top + y).any() && tries > 0);
		if (tries <= 0) continue;

		result.emplace_back(x, y);
	}
	return result;
}
