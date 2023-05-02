#pragma once
#include "BiomeTypes.h"

namespace drft::gen
{
	struct EntityTriplet
	{
		std::string name;
		int number;
		std::string algorithm;
	};

	struct Biome
	{
		BiomeType type;

		using PrototypeList = std::vector<EntityTriplet>;
		std::unordered_map<std::string, PrototypeList> prototypes;
	};
}