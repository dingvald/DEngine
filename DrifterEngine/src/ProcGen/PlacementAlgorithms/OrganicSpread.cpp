#include <pch.h>
#include "OrganicSpread.h"
#include "Spatial/Helpers.h"
#include "Random/RandomNumberGenerator.h"

std::vector<sf::Vector2i> drft::gen::organicSpread(const GenerationContext& ctx, const GenerationParameters& params)
{
	std::vector<sf::Vector2i> positions;
	const int generations = (int)std::get<float>(params.at("Generations"));
	const int startingSeeds = (int)std::get<float>(params.at("StartingSeeds"));
	const int seedsPerGeneration = (int)std::get<float>(params.at("SeedsPerGeneration"));
	const int radius = (int)std::get<float>(params.at("Radius"));

	for (int ss = 0; ss < startingSeeds; ++ss)
	{
		int x = 0;
		int y = 0;
		int safetyCount = 20;
		do {
			--safetyCount;
			x = rng::RandomNumberGenerator::intInRange(0, ctx.area.width - 1);
			y = rng::RandomNumberGenerator::intInRange(0, ctx.area.height - 1);
		} while (ctx.grid.at(ctx.area.left + x, ctx.area.top + y).any() && safetyCount > 0);
		if (safetyCount > 0)
		{
			positions.emplace_back(x, y);
		}
	}

	for (int gen = 0; gen < generations; ++gen)
	{
		std::vector<sf::Vector2i> positionsToAdd;
		for (auto&& position : positions)
		{
			const auto surroundings = spatial::getIntCircleInRadius(position, radius);
			for (int spg = 0; spg < seedsPerGeneration; ++spg)
			{
				const int index = rng::RandomNumberGenerator::intInRange(0, surroundings.size() - 1);
				auto pos = surroundings.at(index);
				if (ctx.grid.at(ctx.area.left + pos.x, ctx.area.top + pos.y).none())
				{
					positionsToAdd.emplace_back(pos);
				}
			}
		}
		positions.insert(positions.end(), positionsToAdd.begin(), positionsToAdd.end());
	}
	return positions;
}
