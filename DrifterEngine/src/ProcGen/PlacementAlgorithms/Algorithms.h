#pragma once
#include "RandomSpread.h"
#include "PerlinSpread.h"
#include "OrganicSpread.h"

namespace drft::gen
{
	using PositionList = std::vector<sf::Vector2i>;
	using GenAlgorithm = std::function<PositionList(int, sf::Vector2i, GenerationParameters)>;

	const std::unordered_map<std::string, GenAlgorithm> String2Algorithm =
	{
		{"RandomSpread", gen::randomSpread},
		{"PerlinSpread", gen::perlinSpread},
		{"OrganicSpread", gen::organicSpread}
	};

	void place(const std::string& name, sf::Vector2i origin, const std::vector<sf::Vector2i>& positions, entt::registry& reg);
}