#pragma once
#include "Biome.h"

class BiomeRegistry
{
public:
	void createBiomesFromJSON(const std::filesystem::path& directoryPath);
	const Biome& get(const std::string& name) const;
	void forEachBiome(std::function<void(const std::string&, const Biome&)> callback) const;

private:
	std::unordered_map<std::string, Biome> _biomes;
	std::unordered_map<std::string, drft::math::Range<float>> _globalRanges;
};

