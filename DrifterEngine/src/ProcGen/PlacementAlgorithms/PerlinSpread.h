#pragma once
#include "GenerationParameters.h"

namespace drft::gen
{
	// Create a number of entities in the given registry with a random position in the given area
	std::vector<sf::Vector2i> perlinSpread(int number, sf::Vector2i bounds, GenerationParameters);
}