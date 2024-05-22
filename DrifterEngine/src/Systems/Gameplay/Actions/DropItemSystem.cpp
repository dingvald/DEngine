#include "pch.h"
#include "DropItemSystem.h"
#include "Components/Components.h"
#include "Spatial/Conversions.h"
#include "Systems/Helpers/ItemDatabase.h"

void drft::system::DropItemSystem::init()
{
	_registry->on_construct<component::action::Drop>().connect<&DropItemSystem::onItemDropped>(this);
	_registry->on_update<component::action::Drop>().connect<&DropItemSystem::onItemDropped>(this);
}

void drft::system::DropItemSystem::update(const float dt)
{
	auto view = _registry->view<component::action::Drop, component::Position>();

	for (auto [entity, items, pos] : view.each())
	{
		for (auto item : items.toDrop)
		{
			auto itemEntity = ItemDatabase::getEntityFromItemID(item);
			_registry->emplace<component::Position>(itemEntity, pos.position);
		}
	}
}

void drft::system::DropItemSystem::onUpdateEnd()
{
	_registry->clear<component::action::Drop>();
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
			registry.patch<component::Container>(entity,
				[itemItr](component::Container& cont)
				{
					cont.contents.erase(itemItr);
				});
		}
	}
}
