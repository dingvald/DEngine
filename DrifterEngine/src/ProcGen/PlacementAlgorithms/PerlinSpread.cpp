#include <pch.h>
#include "PerlinSpread.h"
#include "Random/PerlinNoise.h"
#include "Random/RandomNoise.h"

std::vector<sf::Vector2i> drft::gen::perlinSpread(int seed, sf::Vector2i bounds, GenerationParameters params)
{
	rng::PerlinNoise noise(seed);
	std::vector<sf::Vector2i> result;
	float threshold = params.at("Threshold");

	for (int y = 0; y < bounds.y; ++y)
	{
		for (int x = 0; x < bounds.x; ++x)
		{
			const double dx = (static_cast<double>(x) - 0.5) / 32;
			const double dy = (static_cast<double>(y) - 0.5) / 32;
			const double val = noise.gen(dx, dy);

			if (val > threshold)
			{
				result.emplace_back(x, y);
			}
		}
	}

	return result;
}
