#pragma once
#include "RandomSpread.h"
#include "PerlinSpread.h"
#include "OrganicSpread.h"
#include "NoiseLayerSpread.h"
#include "Fill.h"
#include "Spatial/AutoGrid.h"
#include "Random/NoiseLayer.h"
#include "GenerationContext.h"

namespace drft::gen
{
	using PositionList = std::vector<sf::Vector2i>;
	using GenAlgorithm = std::function<PositionList(const GenerationContext&, const GenerationParameters&)>;

	const std::unordered_map<std::string, GenAlgorithm> String2SpawnAlgorithm =
	{
		{"RandomSpread", gen::randomSpread},
		{"PerlinSpread", gen::perlinSpread},
		{"OrganicSpread", gen::organicSpread},
		{"Fill", gen::fill},
		{"NoiseLayer", gen::noiseLayerSpread}
	};

	void place(const std::string& name, sf::Vector2i origin, sf::Vector2i position, entt::registry& reg);
	void place(const std::string& name, sf::Vector2i origin, const std::vector<sf::Vector2i>& positions, entt::registry& reg);
	void fastFill(const std::string& name, sf::Vector2i origin, entt::registry& reg);
}