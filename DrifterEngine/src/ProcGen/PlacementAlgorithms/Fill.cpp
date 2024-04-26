#include <pch.h>
#include "Fill.h"

std::vector<sf::Vector2i> drft::gen::fill(const GenerationContext& ctx, const GenerationParameters& params)
{
	std::vector<sf::Vector2i> positions;
	
	for (int y = 0; y < ctx.area.height; ++y)
	{
		for (int x = 0; x < ctx.area.width; ++x)
		{
			if (ctx.grid.at(ctx.area.left + x, ctx.area.top + y).any()) continue;
			positions.emplace_back(x, y);
		}
	}

	return positions;
}
