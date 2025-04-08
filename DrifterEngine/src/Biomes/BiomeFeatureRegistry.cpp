#include "pch.h"
#include "BiomeFeatureRegistry.h"
#include <JSON/JSONHelpers.h>

using namespace drft;

void BiomeFeatureRegistry::loadBiomeFeatures(const std::filesystem::path& biomeFeatruresDirectory)
{
	for (const auto& entry : std::filesystem::recursive_directory_iterator(biomeFeatruresDirectory))
	{
		if (entry.is_directory()) continue;

		json::JsonFileWrapper json{ entry.path(), "biomes" };
		if (!json.load())
		{
			//LOG_WARNING("{} could not be loaded", entry);
			continue;
		}
		for (auto&& biomeObj : json.getRoot().GetObject())
		{
			std::string name = biomeObj.name.GetString();
			BiomeFeature feature;
			feature.createFromJson(biomeObj.value);
			_biomeFeatures.emplace(std::move(name), std::move(feature));
		}
	}
}

const BiomeFeature* BiomeFeatureRegistry::get(const std::string& name) const
{
	if (!_biomeFeatures.contains(name)) return nullptr;
	return &_biomeFeatures.at(name);
}
