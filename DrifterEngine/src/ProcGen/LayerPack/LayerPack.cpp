#include "pch.h"
#include "LayerPack.h"

void LayerPack::createFromJson(const rapidjson::Value& json)
{
	if (json.HasMember("layers"))
	{
		for (auto&& layerObj : json["layers"].GetObject())
		{
			entt::id_type layerId = entt::hashed_string{ layerObj.name.GetString() };
			TypedParams newPair;
			if (layerObj.value.HasMember("type"))
			{
				newPair.layerTypeId = entt::hashed_string{ layerObj.value["type"].GetString() };
			}
			if (layerObj.value.HasMember("params"))
			{
				newPair.json.CopyFrom(layerObj.value["params"], newPair.json.GetAllocator());
			}
			_layers.emplace(layerId, std::move(newPair));
		}
	}
}

const std::unordered_map<entt::id_type, LayerPack::TypedParams>& LayerPack::getLayers() const
{
	return _layers;
}
