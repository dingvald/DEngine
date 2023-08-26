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
		std::optional<std::string> pickMachine(int seed) const;
		std::string pickRandomEntity(const std::string& category) const;

		BiomeType type;
		using PrototypeList = std::vector<WildernessPrototype>;
		std::unordered_map<std::string, PrototypeList> prototypes;
		std::unordered_map<std::string, float> machines;
	};
}