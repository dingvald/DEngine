#include "pch.h"
#include "FeatureRegistry.h"
#include <JSON/JSONHelpers.h>
#include <Utility/StandardLogger.h>

#include <ProcGen/Features/ClusterFeature.h>
#include <ProcGen/Features/AutomataFeature.h>

using namespace drft;


FeatureRegistry::FeatureRegistry()
{
	_featureFactory.registerType<ClusterFeature>("cluster_feature");
	_featureFactory.registerType<AutomataFeature>("automata_feature");
}

void FeatureRegistry::loadFeatures(const std::filesystem::path& biomeFeatruresDirectory, const DecoratorFactory& decorators)
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
			const entt::id_type featureId = entt::hashed_string{ featureObj.name.GetString() };
			if (featureObj.value.HasMember("type"))
			{
				type = featureObj.value["type"].GetString();
			}
			else
			{
				LOG_WARNING("Feature {} json does not have \"type\" member", featureObj.name.GetString());
				continue;
			}
			if (featureObj.value.HasMember("params"))
			{
				auto& params = featureObj.value["params"];
				if (auto feature = _featureFactory.build(type))
				{
					feature->createFromJson(params);
					_features.emplace(featureId, std::move(feature));
				}
				else
				{
					LOG_WARNING("Feature type {} does not exist", type);
				}
			}
			else
			{
				LOG_WARNING("Feature {} json does not have \"params\" member", featureObj.name.GetString());
				continue;
			}

			if (_features.contains(featureId) && featureObj.value.HasMember("decorators"))
			{
				for (auto&& decoratorObj : featureObj.value["decorators"].GetArray())
				{
					auto decoratorType = decoratorObj["type"].GetString();
					if (auto decorator = decorators.build(decoratorType))
					{
						decorator->createFromJson(decoratorObj);
						_features.at(featureId)->addDecorator(std::move(decorator));
					}
				}
			}
		}
	}
}

const IFeature* FeatureRegistry::get(entt::id_type id) const
{
	if (!_features.contains(id)) return nullptr;
	return _features.at(id).get();
}
