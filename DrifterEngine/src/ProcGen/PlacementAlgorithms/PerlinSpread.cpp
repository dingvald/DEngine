#include <pch.h>
#include "PerlinSpread.h"
#include "Random/PerlinNoise.h"
#include "Random/RandomNoise.h"

std::vector<sf::Vector2i> drft::gen::perlinSpread(int seed, const spatial::Grid<CellState>& grid, GenerationParameters params)
{
	rng::PerlinNoise noise(seed);
	std::vector<sf::Vector2i> result;
	float threshold = params.at("Threshold");

	for (int y = 0; y < grid.height(); ++y)
	{
		for (int x = 0; x < grid.width(); ++x)
		{
			const double dx = (static_cast<double>(x) - 0.5) / 32;
			const double dy = (static_cast<double>(y) - 0.5) / 32;
			const double val = noise.gen(dx, dy);

			if (val > threshold && grid.at(x,y) == CellState::Free)
			{
				result.emplace_back(x, y);
			}
		}
	}

	return result;
}
