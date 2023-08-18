#pragma once
#include "GenerationParameters.h"
#include "GridEnums.h"

namespace drft::gen
{
	std::vector<sf::Vector2i> fill(const spatial::Grid<int>&, GenerationParameters, int);
}