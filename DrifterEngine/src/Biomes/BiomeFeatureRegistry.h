#pragma once
#include <Biomes/BiomeFeature.h>

class BiomeFeatureRegistry
{
public:
	void loadBiomeFeatures(const std::filesystem::path& biomeFeatruresDirectory);
	const BiomeFeature* get(const std::string& name) const;

private:
	std::unordered_map<std::string, BiomeFeature> _biomeFeatures;
};

