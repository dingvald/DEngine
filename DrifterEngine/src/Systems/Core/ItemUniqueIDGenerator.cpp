#include "pch.h"
#include "ItemUniqueIDGenerator.h"
#include "Components/ItemComponent.h"

std::unordered_map<unsigned long, entt::entity> drft::system::ItemUniqueIDGenerator::_itemToEntityCache;

void drft::system::ItemUniqueIDGenerator::init()
{
	_registry.on_construct<ItemComponent>().connect<&ItemUniqueIDGenerator::onItemAdd>(this);
}

void drft::system::ItemUniqueIDGenerator::save(cereal::JSONOutputArchive& oarchive)
{
	oarchive(cereal::make_nvp("NextAvailableItemID", _nextAvailableID));
}

void drft::system::ItemUniqueIDGenerator::load(cereal::JSONInputArchive& iarchive)
{
	iarchive(cereal::make_nvp("NextAvailableItemID", _nextAvailableID));
}

void drft::system::ItemUniqueIDGenerator::onItemAdd(entt::registry& registry, entt::entity entity)
{
	auto& item = registry.get<ItemComponent>(entity);
	if (item.id == ItemComponent::NONE)
	{
		item.id = _nextAvailableID++;
	}
	ItemUniqueIDGenerator::_itemToEntityCache[item.id] = entity;
}
