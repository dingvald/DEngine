#pragma once
#include "GenerationParameters.h"

namespace drft::gen
{
	// Generates a random list of positions within the given bounds
	std::vector<sf::Vector2i> randomSpread(sf::IntRect area, const spatial::Grid<std::bitset<32>>&, GenerationParameters, int);
}