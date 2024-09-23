#include "pch.h"
#include "CraftItemSystem.h"

#include "Components/Components.h"
#include "Components/ContainerComponent.h"
#include <Components/CraftableComponent.h>
#include "Components/ItemComponent.h"
#include "Components/PositionComponent.h"

#include "Factory/EntityFactory.h"
#include "Systems/Helpers/ItemDatabase.h"
#include "Utility/EntityHelpers.h"


bool drft::system::CraftItemSystem::craftItem(entt::handle crafter, const std::string& itemName)
{
	auto& container = crafter.get<ContainerComponent>();
	auto& factory = crafter.registry()->ctx().get<const EntityFactory&>();
	auto itemPrototype = factory.get(itemName);

	if (auto craftable = itemPrototype.try_get<CraftableComponent>())
	{
		std::vector<ItemComponent::ID> itemIdsToRemove;
		std::vector<entt::entity> itemEntitiesToDestroy;
		bool canCraft = true;
		for (auto&& [matName, amount] : craftable->recipe)
		{
			int count = 0;
			for (auto item : container.contents)
			{
				auto itemEntity = ItemDatabase::getEntityFromItemID(item);
				if (matName.compare(util::getEntityName({ *crafter.registry(), itemEntity})) == 0)
				{
					itemIdsToRemove.push_back(item);
					itemEntitiesToDestroy.push_back(itemEntity);
					++count;
				}

				if (count == amount) break;
			}
			canCraft = (count >= amount);
		}

		if (!canCraft) return false;

		crafter.patch<ContainerComponent>(
			[&itemIdsToRemove](ContainerComponent& cont)
			{
				for (auto item : itemIdsToRemove)
				{
					cont.contents.erase(std::remove(cont.contents.begin(), cont.contents.end(), item), cont.contents.end());
				}
			});

		auto newItem = factory.build(itemName, *crafter.registry());
		newItem.remove<PositionComponent>();
		auto& itemComp = newItem.get<ItemComponent>();

		crafter.patch<ContainerComponent>(
			[&itemComp](ContainerComponent& cont)
			{
				cont.contents.push_back(itemComp.id);
			});

		for (auto e : itemEntitiesToDestroy)
		{
			crafter.registry()->destroy(e);
		}

		return true;
	}

	return false;
}
