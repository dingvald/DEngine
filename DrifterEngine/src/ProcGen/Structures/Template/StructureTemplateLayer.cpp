#include "pch.h"
#include "StructureTemplateLayer.h"
#include <ProcGen/GenerationContext.h>
#include <ProcGen/GenerationFinalizationContext.h>
#include <Utility/StandardLogger.h>
#include <Utility/Vector3Utils.h>

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

	if (json.HasMember("decorators"))
	{
		for (auto&& val : json["decorators"].GetArray())
		{
			DecoratorItem newItem;
			newItem.createFromJson(val);
			_decorators.emplace_back(std::move(newItem));
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
	for (auto&& decorator : _decorators)
	{
		decorator.finalize(context);
	}
}

StructureInstanceLayer StructureTemplateLayer::generate(sf::Vector2i origin, const GenerationContext& context) const
{
	SlotPositionMap map;

	const PrefabRegistry& prefabs = context.registries.prefabs;
	const SelectorContext selectorCtx{ context.seed };

	for (auto&& prefabPoolItem : _prefabPool)
	{
		auto optionalPosition = prefabPoolItem.positionSelector->selectPosition(_area, selectorCtx);
		if (optionalPosition.has_value())
		{
			auto& prefab = prefabs.get(prefabPoolItem.id);
			const sf::Vector3i position3d = 
			{ 
				optionalPosition.value().x + origin.x, 
				optionalPosition.value().y + origin.y, 
				0 
			};
			SlotPositionList positionList = prefab.generate(position3d, context);
			
			// Decorate the prefab
			for (auto&& decoratorItem : _decorators)
			{
				decoratorItem.decorate(positionList, prefab.getTaggedPositions(), context);
			}

			mergeSlotListIntoMap(map, positionList);
		}
	}

	return StructureInstanceLayer{ std::move(map) };
}

void StructureTemplateLayer::PrefabPoolItem::createFromJson(const rapidjson::Value& json)
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

void StructureTemplateLayer::PrefabPoolItem::finalize(const GenerationFinalizationContext& context)
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

void StructureTemplateLayer::DecoratorItem::createFromJson(const rapidjson::Value& json)
{
	_decoratorJson = std::make_unique<JsonValueStorage>(json);
}

void StructureTemplateLayer::DecoratorItem::finalize(const GenerationFinalizationContext& context)
{
	const std::string decoratorName = _decoratorJson->getValue()["type"].GetString();
	decoratorPtr = context.decoratorFactory.build(decoratorName);
	decoratorPtr->createFromJson(_decoratorJson->getValue());
}

void StructureTemplateLayer::DecoratorItem::decorate(SlotPositionList& inOutSlotPositions, const TaggedPositions& taggedPositions, const GenerationContext& context) const
{
	if (!_decoratorPtr) return;

	_decoratorPtr->decorate(inOutSlotPositions, taggedPositions, context);
}