#pragma once
#include "GenerationParameters.h"

namespace drft::gen
{
	// Create a number of entities in the given registry with a random position in the given area
	std::vector<sf::Vector2i> perlinSpread(sf::IntRect area, const spatial::Grid<std::bitset<32>>&, GenerationParameters, int);
}