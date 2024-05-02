#include "pch.h"
#include "RandomSpawn.h"
#include "Random/RandomNumberGenerator.h"

drft::gen::PositionList drft::gen::RandomSpawn::generateSpawnPositions(const GenerationContext& context, const GenerationParameters& params) const
{
	PositionList result;
	int minimum = (int)std::get<float>(params.at("Minimum"));
	int maximum = (int)std::get<float>(params.at("Maximum"));
	int number = rng::RandomNumberGenerator::intInRange(minimum, maximum);
	result.reserve(number);
	
	for (int i = 0; i < number; ++i)
	{
		int x = 0;
		int y = 0;
		int tries = 10;
		do {
			x = rng::RandomNumberGenerator::intInRange(0, context.area.width - 1);
			y = rng::RandomNumberGenerator::intInRange(0, context.area.height - 1);
			--tries;
		} while (context.grid.at(context.area.left + x, context.area.top + y).any() && tries > 0);
		if (tries <= 0) continue;

		result.emplace_back(x, y);
	}
	return result;
}
