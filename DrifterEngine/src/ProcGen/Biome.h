#pragma once
#include "BiomeTypes.h"

namespace drft::gen
{
	struct BiomePrototype
	{
		std::string name;
		std::string algorithm;
		std::unordered_map<std::string, float> params;
	};

	struct Biome
	{
		BiomeType type;
		using PrototypeList = std::vector<BiomePrototype>;
		std::unordered_map<std::string, PrototypeList> prototypes;
	};
}