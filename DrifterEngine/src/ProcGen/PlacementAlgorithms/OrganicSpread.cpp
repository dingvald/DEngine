#include <pch.h>
#include "OrganicSpread.h"
#include "Spatial/Helpers.h"
#include "Random/RandomNumberGenerator.h"

std::vector<sf::Vector2i> drft::gen::organicSpread(int seed, const spatial::Grid<int>& grid, GenerationParameters params)
{
	std::vector<sf::Vector2i> positions;
	const int generations = static_cast<int>(params.at("Generations"));
	const int startingSeeds = static_cast<int>(params.at("StartingSeeds"));
	const int seedsPerGeneration = static_cast<int>(params.at("SeedsPerGeneration"));
	const int radius = static_cast<int>(params.at("Radius"));

	for (int ss = 0; ss < startingSeeds; ++ss)
	{
		int randx = rng::RandomNumberGenerator::intInRange(0, grid.width());
		int randy = rng::RandomNumberGenerator::intInRange(0, grid.height());
		positions.emplace_back(randx, randy);
	}

	for (int gen = 0; gen < generations; ++gen)
	{
		std::vector<sf::Vector2i> positionsToAdd;
		for (auto position : positions)
		{
			auto surroundings = spatial::getIntCircleInRadius(position, radius);
			for (int spg = 0; spg < seedsPerGeneration; ++spg)
			{
				int index = rng::RandomNumberGenerator::intInRange(0, surroundings.size() - 1);
				positionsToAdd.emplace_back(surroundings.at(index));
			}
		}
		positions.insert(positions.end(), positionsToAdd.begin(), positionsToAdd.end());
	}


	return positions;
}
