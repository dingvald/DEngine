#pragma once
#include <JSON/ICreateFromJson.h>

class LayerPack : public ICreateFromJson
{
public:
	struct TypedParams
	{
		entt::id_type layerTypeId;
		rapidjson::Document json;
	};
	void createFromJson(const rapidjson::Value& json) override;

	const std::unordered_map<entt::id_type, TypedParams>& getLayers() const;

private:
	std::unordered_map<entt::id_type, TypedParams> _layers;
};