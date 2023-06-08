#include "pch.h"
#include "ItemUniqueIDGenerator.h"
#include "Components/Components.h"

void drft::system::ItemUniqueIDGenerator::init()
{
	registry->on_construct<component::Item>().connect<&ItemUniqueIDGenerator::onItemAdd>(this);
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
	auto& item = registry.get<component::Item>(entity);
	if (item.id == component::Item::NONE)
	{
		item.id = _nextAvailableID++;
	}
}
