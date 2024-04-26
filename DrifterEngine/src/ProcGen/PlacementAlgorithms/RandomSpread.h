#pragma once
#include "GenerationContext.h"

namespace drft::gen
{
	// Generates a random list of positions within the given bounds
	std::vector<sf::Vector2i> randomSpread(const GenerationContext& ctx, const GenerationParameters& params);
}