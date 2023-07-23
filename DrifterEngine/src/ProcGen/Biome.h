#pragma once
#include "BiomeTypes.h"

namespace drft::gen
{
	struct EnvironmentGeneration
	{
		std::string algorithm;
		std::unordered_map<std::string, float> params;
	};
	struct WildlifeChance
	{
		float max;
		float chance;
	};
	
	class Biome
	{
	public:
		using Environment = std::unordered_map<std::string, EnvironmentGeneration>;
		using WildlifeTier = std::unordered_map<std::string, WildlifeChance>;
		using SpawnChance = float;

		using Environmentals = std::unordered_map<std::string, Environment>;
		using Wildlife = std::unordered_map<std::string, WildlifeTier>;
		using Machines = std::unordered_map<std::string, SpawnChance>;

		const Environmentals& environmentals() const { return _environmentals; };
		const Wildlife& wildlife() const { return _wildlife; };
		const Machines& machines() const { return _machines; };

		std::vector<std::string> pickRandomMachines() const;
		std::string pickRandomWildlife(const std::string& category = "any") const;
		std::string pickRandomEnvironmental(const std::string& category = "any") const;

	private:
		friend class WorldGenerator;
		Environmentals _environmentals;
		Wildlife _wildlife;
		Machines _machines;
	};
}