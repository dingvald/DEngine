#pragma once
#include "GenerationParameters.h"
#include "GridEnums.h"

namespace drft::gen
{
	// Generates a random list of positions within the given bounds
	std::vector<sf::Vector2i> randomSpread(int seed, const spatial::Grid<CellState>& grid, GenerationParameters);
}