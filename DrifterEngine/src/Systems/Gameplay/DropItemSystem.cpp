#include "pch.h"
#include "DropItemSystem.h"
#include "Components/Components.h"
#include "Spatial/Conversions.h"
#include "Utility/ItemIDToEntityID.h"

void drft::system::DropItemSystem::init()
{
	registry->on_construct<component::action::Drop>().connect<&DropItemSystem::onItemDropped>(this);
	registry->on_update<component::action::Drop>().connect<&DropItemSystem::onItemDropped>(this);
}

void drft::system::DropItemSystem::update(const float dt)
{
	auto view = registry->view<component::action::Drop, component::Position>();

	for (auto [entity, items, pos] : view.each())
	{
		for (auto item : items.toDrop)
		{
			auto itemEntity = util::ItemIDToEntityID(item, *registry);
			registry->emplace<component::Position>(itemEntity, pos.position, static_cast<int>(spatial::Layer::Item));
		}

		registry->remove<component::action::Drop>(entity);
	}
}

void drft::system::DropItemSystem::onItemDropped(entt::registry& registry, entt::entity entity)
{
	auto& container = registry.get<component::Container>(entity);
	auto& dropItems = registry.get<component::action::Drop>(entity);

	for (auto item : dropItems.toDrop)
	{
		auto itemItr = std::find(container.contents.begin(), container.contents.end(), item);
		if (itemItr != container.contents.end())
		{
			container.contents.erase(itemItr);
		}
	}
}
