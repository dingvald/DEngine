#include <pch.h>
#include "Fill.h"

std::vector<sf::Vector2i> drft::gen::fill(int seed, const spatial::Grid<CellState>& grid, GenerationParameters)
{
	std::vector<sf::Vector2i> positions;
	
	for (int y = 0; y < grid.height(); ++y)
	{
		for (int x = 0; x < grid.width(); ++x)
		{
			if (grid.at(x, y) == CellState::Free)
			{
				positions.emplace_back(x, y);
			}
		}
	}

	return positions;
}
