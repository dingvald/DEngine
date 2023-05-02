#pragma once
#include "GenerationParameters.h"

namespace drft::gen
{
	// Generates a random list of positions within the given bounds
	std::vector<sf::Vector2i> randomSpread(int number, sf::Vector2i bounds, GenerationParameters);
}