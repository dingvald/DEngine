#pragma once
#include "GenerationParameters.h"
#include "Random/NoiseLayer.h"
#include "Spatial/AutoGrid.h"

namespace drft::gen
{
	struct GenerationContext
	{
		sf::IntRect area;
		const spatial::AutoGrid<std::bitset<32>>& grid;
		const std::unordered_map<std::string, rng::NoiseLayer>& noiseLayers;
		unsigned int seed;
	};
}