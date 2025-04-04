#include "pch.h"
#include "LayerPackRegistry.h"
#include <JSON/JSONHelpers.h>
#include <Utility/StandardLogger.h>

using namespace drft;

static const LayerPack EmptyLayerPack = {};

void LayerPackRegistry::loadLayerPacks(const std::filesystem::path& layerPackDirectory)
{
	for (const auto& entry : std::filesystem::recursive_directory_iterator(layerPackDirectory))
	{
		if (entry.is_directory()) continue;

		json::JsonFileWrapper json{ entry.path(), "layer_pack" };
		if (!json.load())
		{
			//LOG_WARNING("{} could not be loaded", entry);
			continue;
		}
		for (auto&& packObj : json.getRoot().GetObject()) 
		{
			entt::id_type name = entt::hashed_string{ packObj.name.GetString() };
			LayerPack layerPack;
			layerPack.createFromJson(packObj.value);
			_packs.emplace(std::move(name), std::move(layerPack));
		}
	}
}

const LayerPack& LayerPackRegistry::get(entt::id_type id) const
{
	if (!_packs.contains(id))
	{
		LOG_WARNING("Could not find layer pack with id {} - Returning empty pack", id);
		return EmptyLayerPack;
	}
	return _packs.at(id);
}
