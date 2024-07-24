#include "pch.h"
#include "CraftItemSystem.h"

#include "Components/Components.h"
#include "Components/ContainerComponent.h"
#include "Components/ItemComponent.h"
#include "Components/PositionComponent.h"

#include "Factory/EntityFactory.h"
#include "Systems/Helpers/ItemDatabase.h"
#include "Utility/EntityHelpers.h"

void drft::system::CraftItemSystem::init()
{
	_registry.on_construct<component::action::Craft>().connect<&CraftItemSystem::onCraftItem>(this);
	_registry.on_update<component::action::Craft>().connect<&CraftItemSystem::onCraftItem>(this);
}

void drft::system::CraftItemSystem::onUpdateEnd()
{
	_registry.clear<component::action::Craft>();
}

void drft::system::CraftItemSystem::onCraftItem(entt::registry& registry, entt::entity entity)
{
	auto& container = registry.get<ContainerComponent>(entity);
	auto& craftAction = registry.get<component::action::Craft>(entity);

	std::vector<ItemComponent::ID> toRemove;
	for (auto&& [matName, amount] : craftAction.recipe)
	{
		int count = 0;
		for (auto item : container.contents)
		{
			auto itemEntity = ItemDatabase::getEntityFromItemID(item);
			if (matName.compare(util::getEntityName({ registry, itemEntity })) == 0)
			{
				toRemove.push_back(item);
				registry.destroy(itemEntity);
				++count;
			}

			if (count == amount) break;
		}
	}

	registry.patch<ContainerComponent>(entity,
		[&toRemove](ContainerComponent& cont)
		{
			for (auto item : toRemove)
			{
				cont.contents.erase(std::remove(cont.contents.begin(), cont.contents.end(), item), cont.contents.end());
			}
		});

	const auto& factory = registry.ctx().get<const EntityFactory&>();

	auto newItem = factory.build(craftAction.itemName, registry);
	newItem.remove<PositionComponent>();
	auto& itemComp = newItem.get<ItemComponent>();

	registry.patch<ContainerComponent>(entity,
		[&itemComp](ContainerComponent& cont)
		{
			cont.contents.push_back(itemComp.id);
		});
}
