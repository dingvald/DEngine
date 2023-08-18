#include <pch.h>
#include "Fill.h"

std::vector<sf::Vector2i> drft::gen::fill(const spatial::Grid<int>& grid, GenerationParameters params, int seed)
{
	std::vector<sf::Vector2i> positions;
	
	for (int y = 0; y < grid.height(); ++y)
	{
		for (int x = 0; x < grid.width(); ++x)
		{
			if (grid.at(x, y) == 0)
			{
				positions.emplace_back(x, y);
			}
		}
	}

	return positions;
}
