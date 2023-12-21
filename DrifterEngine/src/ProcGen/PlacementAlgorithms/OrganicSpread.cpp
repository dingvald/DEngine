#include <pch.h>
#include "OrganicSpread.h"
#include "Spatial/Helpers.h"
#include "Random/RandomNumberGenerator.h"

std::vector<sf::Vector2i> drft::gen::organicSpread(const spatial::Grid<std::bitset<32>>& grid, GenerationParameters params, int seed)
{
	std::vector<sf::Vector2i> positions;
	const int generations = static_cast<int>(params.at("Generations"));
	const int startingSeeds = static_cast<int>(params.at("StartingSeeds"));
	const int seedsPerGeneration = static_cast<int>(params.at("SeedsPerGeneration"));
	const int radius = static_cast<int>(params.at("Radius"));

	for (int ss = 0; ss < startingSeeds; ++ss)
	{
		int x = 0;
		int y = 0;
		int safetyCount = 20;
		do {
			--safetyCount;
			x = rng::RandomNumberGenerator::intInRange(0, grid.width() - 1);
			y = rng::RandomNumberGenerator::intInRange(0, grid.height() - 1);
		} while (grid.at(x, y).any() && safetyCount > 0);
		if (safetyCount > 0)
		{
			positions.emplace_back(x, y);
		}
	}

	for (int gen = 0; gen < generations; ++gen)
	{
		std::vector<sf::Vector2i> positionsToAdd;
		for (auto position : positions)
		{
			const auto surroundings = spatial::getIntCircleInRadius(position, radius);
			for (int spg = 0; spg < seedsPerGeneration; ++spg)
			{
				const int index = rng::RandomNumberGenerator::intInRange(0, surroundings.size() - 1);
				auto pos = surroundings.at(index);
				if (grid.contains(pos.x, pos.y) && grid.at(pos.x, pos.y).none())
				{
					positionsToAdd.emplace_back(pos);
				}
			}
		}
		positions.insert(positions.end(), positionsToAdd.begin(), positionsToAdd.end());
	}
	return positions;
}
