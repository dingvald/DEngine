#include "pch.h"
#include "StructureTemplateLayer.h"
#include <ProcGen/GenerationContext.h>
#include <ProcGen/GenerationFinalizationContext.h>
#include <Utility/StandardLogger.h>
#include <Utility/Vector3Utils.h>

void PrefabPoolItem::createFromJson(const rapidjson::Value& json)
{
	if (json.HasMember("id"))
	{
		id = entt::hashed_string{ json["id"].GetString() };
	}
	else
	{
		LOG_ERROR("Item in 'prefab_pool' does not have an 'id'");
		return;
	}

	if (json.HasMember("position_selector"))
	{
		_positionSelectorJson = std::make_unique<JsonValueStorage>(json["position_selector"]);
	}
}

void PrefabPoolItem::finalize(const GenerationFinalizationContext& context)
{
	if (!_positionSelectorJson->getValue().HasMember("type"))
	{
		LOG_ERROR("'position_selector' requires a 'type' field");
		return;
	}
	if (!_positionSelectorJson->getValue().HasMember("params"))
	{
		LOG_ERROR("'position_selector' requires a 'params' field");
		return;
	}

	const std::string positionSelectorType = _positionSelectorJson->getValue()["type"].GetString();
	
	positionSelector = context.positionSelectorFactory.build(positionSelectorType);
	if (positionSelector)
	{
		positionSelector->createFromJson(_positionSelectorJson->getValue()["params"]);
	}
	else
	{
		LOG_ERROR("Could not find 'position_selector' with name {}", positionSelectorType);
	}

	// Erase the old Value by resetting the pointer
	_positionSelectorJson.reset();
}

void StructureTemplateLayer::createFromJson(const rapidjson::Value& json)
{
	if (json.HasMember("max_area"))
	{
		int x = json["max_area"].GetArray()[0].GetInt();
		int y = json["max_area"].GetArray()[1].GetInt();
		_area.size = { x, y };
	}
	else
	{
		LOG_ERROR("Structure 'layers' requires a 'max_area'");
	}

	if (json.HasMember("prefab_pool"))
	{
		for (auto&& val : json["prefab_pool"].GetArray())
		{
			PrefabPoolItem newPrefabPoolItem;
			newPrefabPoolItem.createFromJson(val);
			_prefabPool.emplace_back(std::move(newPrefabPoolItem));
		}
	}
}

void StructureTemplateLayer::finalize(const GenerationFinalizationContext& context)
{
	for (auto&& prefabPoolItem : _prefabPool)
	{
		prefabPoolItem.finalize(context);
	}
	for (auto&& prefabPoolItem : _prefabPool)
	{
		auto& prefab = context.generationRegistries.prefabs.get(prefabPoolItem.id);
		auto areaIntersection = _area.findIntersection(prefab.getArea());
		if (!areaIntersection.has_value())
		{
			LOG_WARNING("prefab {} is too large to fit into area dimensions {}", prefabPoolItem.id, drft::Vector3Utils::toString(_area.size));
		}
	}
}

StructureInstanceLayer StructureTemplateLayer::generate(const GenerationContext& context) const
{
	SlotPositionMap map;

	return StructureInstanceLayer{ std::move(map) };
}
