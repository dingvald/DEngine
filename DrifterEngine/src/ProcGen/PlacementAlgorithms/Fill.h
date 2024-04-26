#pragma once
#include "GenerationContext.h"

namespace drft::gen
{
	std::vector<sf::Vector2i> fill(const GenerationContext& ctx, const GenerationParameters& params);
}