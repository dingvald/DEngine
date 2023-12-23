#include <pch.h>
#include "Fill.h"

std::vector<sf::Vector2i> drft::gen::fill(sf::IntRect area, const spatial::Grid<std::bitset<32>>& grid, GenerationParameters params, int seed)
{
	std::vector<sf::Vector2i> positions;
	
	for (int y = 0; y < area.height; ++y)
	{
		for (int x = 0; x < area.width; ++x)
		{
			if (grid.at(area.left + x, area.top + y).any()) continue;
			positions.emplace_back(x, y);
		}
	}

	return positions;
}
