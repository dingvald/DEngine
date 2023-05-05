#pragma once
#include "GenerationParameters.h"

namespace drft::gen
{
	std::vector<sf::Vector2i> fill(int seed, const spatial::Grid<int>& grid, GenerationParameters);
}