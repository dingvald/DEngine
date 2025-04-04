#pragma once
#include "Biome.h"
#include <JSON/ICreateFromJson.h>

class BiomeRegistry
{
public:
	BiomeRegistry();

	void loadBiomes(const std::filesystem::path& biomesDirectory);
	const Biome& get(const std::string& name) const;
	void forEachBiome(std::function<void(const std::string&, const Biome&)> callback) const;

private:
	std::unordered_map<std::string, Biome> _biomes;
};

