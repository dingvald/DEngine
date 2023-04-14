#include "pch.h"
#include "ItemUniqueIDGenerator.h"
#include "Components/Components.h"

void drft::system::ItemUniqueIDGenerator::init()
{
	registry->on_construct<component::Item>().connect<&ItemUniqueIDGenerator::onItemAdd>(this);
}

void drft::system::ItemUniqueIDGenerator::onItemAdd(entt::registry& registry, entt::entity entity)
{
	auto& item = registry.get<component::Item>(entity);
	item.id = _nextAvailableID++;
}
