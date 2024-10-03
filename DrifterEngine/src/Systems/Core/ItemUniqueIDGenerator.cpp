#include "pch.h"
#include "ItemUniqueIDGenerator.h"
#include "Components/ItemComponent.h"
#include <Components/ItemIDTrackerComponent.h>

void drft::system::ItemUniqueIDGenerator::init()
{
	_registry.on_construct<ItemComponent>().connect<&ItemUniqueIDGenerator::onItemAdd>(this);
	_registry.on_construct<ItemIDTrackerComponent>().connect<&ItemUniqueIDGenerator::onItemIDTrackerAdded>(this);
}

void drft::system::ItemUniqueIDGenerator::onStart()
{
	if (_idTracker == entt::null)
	{
		_idTracker = _registry.create();
		_registry.emplace<ItemIDTrackerComponent>(_idTracker);
	}
}

void drft::system::ItemUniqueIDGenerator::onItemAdd(entt::registry& registry, entt::entity entity)
{
	auto& itemComponent = registry.get<ItemComponent>(entity);
	if (itemComponent.id == ItemComponent::NONE)
	{
		auto& maxItemIDComponent = _registry.get<ItemIDTrackerComponent>(_idTracker);
		itemComponent.id = maxItemIDComponent.maxID;
		maxItemIDComponent.maxID++;
	}
	
	_itemToEntityCache[itemComponent.id] = entity;
}

void drft::system::ItemUniqueIDGenerator::onItemIDTrackerAdded(entt::registry& registry, entt::entity entity)
{
	_idTracker = entity;
}

