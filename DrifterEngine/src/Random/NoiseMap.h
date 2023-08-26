#pragma once

#include "Spatial/Grid.h"

namespace drft::rng
{
	class NoiseMap
	{
	public:
		static spatial::Grid<double> generate(sf::Vector2i dimensions, sf::Vector2i repetitions, int seed, int octaves = 8, float lacunarity = 2.0, float gain = 0.5);
	};
}



