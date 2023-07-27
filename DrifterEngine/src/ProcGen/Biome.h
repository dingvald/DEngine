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
	struct JSONMachine
	{
		float chance = 0.f;
		std::unordered_map<std::string, std::string> params;
	};
	
	class Biome
	{
	public:
		using Environment = std::unordered_map<std::string, EnvironmentGeneration>;
		using WildlifeTier = std::unordered_map<std::string, WildlifeChance>;

		using Environmentals = std::unordered_map<std::string, Environment>;
		using Wildlife = std::unordered_map<std::string, WildlifeTier>;
		using Machines = std::unordered_map<std::string, JSONMachine>;

		const Environmentals& environmentals() const { return _environmentals; };
		const Wildlife& wildlife() const { return _wildlife; };
		const Machines& machines() const { return _machines; };

		std::vector<std::string> pickRandomMachines() const;
		std::optional<std::string> pickRandomWildlife(std::vector<std::pair<std::string, float>> categories) const;
		std::optional<std::string> pickRandomEnvironmental(std::vector<std::string> categories) const;

	private:
		std::optional<std::string> _pickRandomWildlife(const std::string& category) const;
		std::optional<std::string> _pickRandomEnvironmental(const std::string& category) const;

	private:
		friend class WorldGenerator;
		Environmentals _environmentals;
		Wildlife _wildlife;
		Machines _machines;
	};
}