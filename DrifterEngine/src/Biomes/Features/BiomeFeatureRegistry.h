#pragma once
#include <Biomes/Features/IBiomeFeature.h>
#include <Factory/Factory.h>

class BiomeFeatureRegistry
{
public:
	BiomeFeatureRegistry();

	void loadBiomeFeatures(const std::filesystem::path& biomeFeatruresDirectory);
	const IBiomeFeature* get(entt::id_type id) const;

private:
	std::unordered_map<entt::id_type, IBiomeFeature::Ptr> _biomeFeatures;
	Factory<IBiomeFeature> _featureFactory;
};

