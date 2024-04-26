#include <pch.h>
#include "PerlinSpread.h"
#include "Random/PerlinNoise.h"
#include "Random/RandomNoise.h"

std::vector<sf::Vector2i> drft::gen::perlinSpread(const GenerationContext& ctx, const GenerationParameters& params)
{
	rng::PerlinNoise noise(ctx.seed);
	std::vector<sf::Vector2i> result;
	float threshold = std::get<float>(params.at("Threshold"));

	for (int y = 0; y < ctx.area.height; ++y)
	{
		for (int x = 0; x < ctx.area.width; ++x)
		{
			const double dx = (static_cast<double>(x) - 0.5) / 32;
			const double dy = (static_cast<double>(y) - 0.5) / 32;
			const double val = noise.gen(dx, dy);

			if (val > threshold && ctx.grid.at(ctx.area.left + x, ctx.area.top + y).none())
			{
				result.emplace_back(x, y);
			}
		}
	}

	return result;
}
