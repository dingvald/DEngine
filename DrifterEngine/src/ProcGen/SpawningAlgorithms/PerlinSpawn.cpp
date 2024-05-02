#include "pch.h"
#include "PerlinSpawn.h"
#include "Random/PerlinNoise.h"

drft::gen::PositionList drft::gen::PerlinSpawn::generateSpawnPositions(const GenerationContext& context, const GenerationParameters& params) const
{
	rng::PerlinNoise noise(context.seed);
	PositionList result;
	float threshold = std::get<float>(params.at("Threshold"));

	for (int y = 0; y < context.area.height; ++y)
	{
		for (int x = 0; x < context.area.width; ++x)
		{
			const double dx = (static_cast<double>(x) - 0.5) / 32;
			const double dy = (static_cast<double>(y) - 0.5) / 32;
			const double val = noise.gen(dx, dy);

			if (val > threshold && context.grid.at(context.area.left + x, context.area.top + y).none())
			{
				result.emplace_back(x, y);
			}
		}
	}

	return result;
}
