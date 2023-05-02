#include <pch.h>
#include "PerlinSpread.h"
#include "Random/PerlinNoise.h"
#include "Random/RandomNoise.h"

static constexpr double THRESHOLD = 0.6;

std::vector<sf::Vector2i> drft::gen::perlinSpread(int number, sf::Vector2i bounds, GenerationParameters p)
{
	rng::PerlinNoise noise(rng::noise(p.params.at("Seed")));
	std::vector<sf::Vector2i> result;

	for (int y = 0; y < bounds.y; ++y)
	{
		for (int x = 0; x < bounds.x; ++x)
		{
			double dx = (static_cast<double>(x) - 0.5) / 32;
			double dy = (static_cast<double>(y) - 0.5) / 32;
			auto val = noise.gen(dx, dy);

			if (val > THRESHOLD)
			{
				result.emplace_back(x, y);
			}
		}
	}

	return result;
}
