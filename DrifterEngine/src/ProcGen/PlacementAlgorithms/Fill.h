#pragma once
#include "GenerationParameters.h"
#include "GridEnums.h"

namespace drft::gen
{
	std::vector<sf::Vector2i> fill(int seed, const spatial::Grid<CellState>& grid, GenerationParameters);
}