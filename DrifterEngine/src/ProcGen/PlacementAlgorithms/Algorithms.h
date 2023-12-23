#pragma once
#include "RandomSpread.h"
#include "PerlinSpread.h"
#include "OrganicSpread.h"
#include "Fill.h"
#include "Spatial/Grid.h"

namespace drft::gen
{
	using PositionList = std::vector<sf::Vector2i>;
	using GenAlgorithm = std::function<PositionList(sf::IntRect, const spatial::Grid<std::bitset<32>>&, GenerationParameters, int)>;

	const std::unordered_map<std::string, GenAlgorithm> String2Algorithm =
	{
		{"RandomSpread", gen::randomSpread},
		{"PerlinSpread", gen::perlinSpread},
		{"OrganicSpread", gen::organicSpread},
		{"Fill", gen::fill}
	};

	void place(const std::string& name, sf::Vector2i origin, sf::Vector2i position, entt::registry& reg);
	void place(const std::string& name, sf::Vector2i origin, const std::vector<sf::Vector2i>& positions, entt::registry& reg);
	void fastFill(const std::string& name, sf::Vector2i origin, entt::registry& reg);
}