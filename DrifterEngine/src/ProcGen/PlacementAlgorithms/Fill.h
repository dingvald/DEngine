#pragma once
#include "GenerationParameters.h"

namespace drft::gen
{
	std::vector<sf::Vector2i> fill(sf::IntRect area, const spatial::Grid<std::bitset<32>>&, GenerationParameters, int);
}