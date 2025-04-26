#include "pch.h"
#include "DetermineCraftableItemsSystem.h"
#include "Components/ContainerComponent.h"
#include "Components/CraftableComponent.h"
#include "Components/MyCraftableItemsComponent.h"

#include "Factory/EntityFactory.h"
#include "Systems/Helpers/ItemDatabase.h"
#include <Utility/EntityAccessors/GetEntityName.h>

void drft::system::DetermineCraftableItemsSystem::init()
{
	_registry.on_construct<ContainerComponent>().connect<&DetermineCraftableItemsSystem::onContainerUpdated>(this);
	_registry.on_update<ContainerComponent>().connect<&DetermineCraftableItemsSystem::onContainerUpdated>(this);
}

void drft::system::DetermineCraftableItemsSystem::onContainerUpdated(entt::registry& registry, entt::entity entity)
{
	const auto& prototypeReg = registry.ctx().get<const EntityFactory&>().prototypes();
	auto craftableView = prototypeReg.view<CraftableComponent>();
	auto& container = registry.get<ContainerComponent>(entity);
	std::unordered_map<std::string, int> inventoryContents;

	std::vector<entt::entity> craftables;
	std::vector<entt::entity> partialCraftables;

	for (auto itemID : container.contents)
	{
		auto itemEntity = ItemDatabase::getEntityFromItemID(itemID);
		++inventoryContents[util::getEntityName({ registry, itemEntity })];
	}

	for (auto [item, craftable] : craftableView.each())
	{
		bool hasSome = false;
		bool hasAll = true;
		for (auto&& [matName, quantity] : craftable.recipe)
		{
			if (inventoryContents.contains(matName))
			{
				hasSome = true;
				if (inventoryContents.at(matName) < static_cast<int>(quantity))
				{
					hasAll = false;
				}
			}
			else
			{
				hasAll = false;
			}
		}

		if (hasAll)
		{
			craftables.push_back(item);
		}
		else if (hasSome)
		{
			partialCraftables.push_back(item);
		}
	}

	registry.emplace_or_replace<MyCraftableItemsComponent>(entity, craftables, partialCraftables);
}
