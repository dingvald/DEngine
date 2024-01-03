#pragma once
#include "GenerationParameters.h"

namespace drft::gen
{
	// Generates a random list of positions within the given bounds
	std::vector<sf::Vector2i> randomSpread(sf::IntRect area, const spatial::AutoGrid<std::bitset<32>>& grid, GenerationParameters, int);
}