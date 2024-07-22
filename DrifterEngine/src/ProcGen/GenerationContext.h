#pragma once

#include "Random/NoiseLayer.h"
#include "Spatial/AutoGrid.h"

#include <bitset>
#include <SFML/Graphics/Rect.hpp>
#include <EnTT/entity/registry.hpp>
#include <EnTT/container/dense_map.hpp>

#include <Utility/stdHashing.h>

namespace drft::gen
{
	using EntityPositionMap = std::unordered_map<std::string, entt::dense_set<sf::Vector2i>>;

	struct GenerationContext
	{
		const sf::IntRect area;
		EntityPositionMap& entityPositions;
		spatial::AutoGrid<entt::dense_set<entt::id_type>>& grid;
		const std::unordered_map<std::string, rng::NoiseLayer>& noiseLayers;
		const unsigned int seed;
	};
}