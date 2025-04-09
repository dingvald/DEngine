#include "pch.h"
#include "BiomeFeatureRegistry.h"
#include <JSON/JSONHelpers.h>
#include <Utility/StandardLogger.h>

#include <Biomes/Features/ClusterBiomeFeature.h>

using namespace drft;


BiomeFeatureRegistry::BiomeFeatureRegistry()
{
	_featureFactory.registerType<ClusterBiomeFeature>("cluster_feature");
}

void BiomeFeatureRegistry::loadBiomeFeatures(const std::filesystem::path& biomeFeatruresDirectory)
{
	for (const auto& entry : std::filesystem::recursive_directory_iterator(biomeFeatruresDirectory))
	{
		if (entry.is_directory()) continue;

		json::JsonFileWrapper json{ entry.path(), "biome_features" };
		if (!json.load())
		{
			LOG_WARNING("{} could not be loaded", entry.path().string());
			continue;
		}
		for (auto&& featureObj : json.getRoot().GetObject())
		{
			std::string type = "none";
			if (featureObj.value.HasMember("type"))
			{
				type = featureObj.value["type"].GetString();
			}
			if (auto feature = _featureFactory.build(type))
			{
				feature->createFromJson(featureObj.value);
				_biomeFeatures.emplace(entt::hashed_string{ featureObj.name.GetString() }, std::move(feature));
			}
			else
			{
				LOG_WARNING("Feature type {} does not exist", type);
			}
		}
	}
}

const IBiomeFeature* BiomeFeatureRegistry::get(entt::id_type id) const
{
	if (!_biomeFeatures.contains(id)) return nullptr;
	return _biomeFeatures.at(id).get();
}
