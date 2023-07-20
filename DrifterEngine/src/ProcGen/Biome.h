#pragma once
#include "BiomeTypes.h"

namespace drft::gen
{
	struct WildernessPrototype
	{
		std::string name;
		std::string algorithm;
		std::unordered_map<std::string, float> params;
	};

	struct Biome
	{
		std::vector<std::string> pickMachines(int seed) const;
		std::string pickRandomEntityFromCategory(const std::string& category) const;

		BiomeType type;
		using PrototypeList = std::vector<WildernessPrototype>;
		std::unordered_map<std::string, PrototypeList> wildernessPrototypes;
		std::unordered_map<std::string, float> possibleElites;
		std::unordered_map<std::string, float> possibleMachines;
	};
}