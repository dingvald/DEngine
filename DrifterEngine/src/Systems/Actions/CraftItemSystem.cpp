#include "pch.h"
#include "CraftItemSystem.h"

#include "Components/ContainerComponent.h"
#include <Components/CraftableComponent.h>
#include "Components/ItemComponent.h"
#include "Components/PositionComponent.h"

#include "Factory/EntityFactory.h"
#include "Systems/Helpers/ItemDatabase.h"
#include <Utility/EntityAccessors/GetEntityName.h>
#include <Utility/StandardLogger.h>


bool drft::system::CraftItemSystem::craftItem(entt::handle crafter, const std::string& itemName)
{
	if (!crafter.all_of<ContainerComponent>())
	{
		warning_logger << "Warning: Entity without a container is attempting to craft an item." << std::endl;
		return false;
	}

	auto& factory = crafter.registry()->ctx().get<const EntityFactory&>();
	auto itemPrototype = factory.get(itemName);

	if (!itemPrototype.valid())
	{
		warning_logger << "Warning: Trying to craft unknown item " << itemName << std::endl;
		return false;
	}

	if (auto craftable = itemPrototype.try_get<CraftableComponent>())
	{
		std::vector<ItemComponent::ID> itemIdsToRemove;
		std::vector<entt::entity> itemEntitiesToDestroy;

		bool canCraft = true;
		auto& container = crafter.get<ContainerComponent>();
		for (auto&& ingredient : craftable->recipe)
		{
			const auto& ingredientName = ingredient.getEntityName();
			const int amount = ingredient.getAmount();

			int count = 0;
			for (auto&& item : container.contents)
			{
				auto itemEntity = ItemDatabase::getEntityFromItemID(item);
				auto& itemName = util::getEntityName({ *crafter.registry(), itemEntity });
				if (ingredientName == itemName)
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
