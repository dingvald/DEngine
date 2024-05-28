#include "pch.h"
#include "DropItemSystem.h"
#include "Components/Components.h"
#include "Components/PositionComponent.h"
#include "Components/ContainerComponent.h"
#include "Spatial/Conversions.h"
#include "Systems/Helpers/ItemDatabase.h"

void drft::system::DropItemSystem::init()
{
	_registry->on_construct<component::action::Drop>().connect<&DropItemSystem::onItemDropped>(this);
	_registry->on_update<component::action::Drop>().connect<&DropItemSystem::onItemDropped>(this);
}

void drft::system::DropItemSystem::update(const float dt)
{
	auto view = _registry->view<component::action::Drop, PositionComponent>();

	for (auto [entity, dropAction, pos] : view.each())
	{
		for (auto item : dropAction.toDrop)
		{
			auto itemEntity = ItemDatabase::getEntityFromItemID(item);
			_registry->emplace<PositionComponent>(itemEntity, pos.position);
		}
	}
}

void drft::system::DropItemSystem::onUpdateEnd()
{
	_registry->clear<component::action::Drop>();
}

void drft::system::DropItemSystem::onItemDropped(entt::registry& registry, entt::entity entity)
{
	auto& container = registry.get<ContainerComponent>(entity);
	auto& dropAction = registry.get<component::action::Drop>(entity);

	for (auto item : dropAction.toDrop)
	{
		auto itemItr = std::find(container.contents.begin(), container.contents.end(), item);
		if (itemItr != container.contents.end())
		{
			registry.patch<ContainerComponent>(entity,
				[itemItr](ContainerComponent& cont)
				{
					cont.contents.erase(itemItr);
				});
		}
	}
}
