#pragma once
#include "BiomeTypes.h"

namespace drft::gen
{
	struct Biome
	{
		BiomeType type;

		using PrototypePair = std::pair<std::string, int>;
		using PrototypeList = std::vector<PrototypePair>;
		std::unordered_map<std::string, PrototypeList> prototypes;
	};
}