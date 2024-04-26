#pragma once
#include "GenerationContext.h"

namespace drft::gen
{
	// Uses the world's noise map values to choose spawning location
	std::vector<sf::Vector2i> noiseLayerSpread(const GenerationContext& ctx, const GenerationParameters& params);
}