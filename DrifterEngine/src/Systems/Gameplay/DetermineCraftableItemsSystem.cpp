#include "pch.h"
#include "DetermineCraftableItemsSystem.h"
#include "Components/Components.h"
#include "Factory/EntityFactory.h"
#include "Utility/ItemIDToEntityID.h"
#include "Utility/EntityHelpers.h"

void drft::system::DetermineCraftableItemsSystem::init()
{
	registry->on_construct<component::Container>().connect<&DetermineCraftableItemsSystem::onContainerUpdated>(this);
	registry->on_update<component::Container>().connect<&DetermineCraftableItemsSystem::onContainerUpdated>(this);
}

void drft::system::DetermineCraftableItemsSystem::onContainerUpdated(entt::registry& registry, entt::entity entity)
{
	const auto& prototypeReg = registry.ctx().get<const EntityFactory&>().prototypes();
	auto craftableView = prototypeReg.view<component::Craftable>();
	auto& container = registry.get<component::Container>(entity);
	std::unordered_map<std::string, int> inventoryContents;

	std::vector<entt::entity> craftables;
	std::vector<entt::entity> partialCraftables;

	for (auto itemID : container.contents)
	{
		auto itemEntity = util::ItemIDToEntityID(itemID, registry);
		++inventoryContents[util::getEntityName({ registry, itemEntity })];
	}

	for (auto [item, craftable] : craftableView.each())
	{
		bool hasSome = false;
		bool hasAll = true;
		for (auto&& [matName, quant] : craftable.recipe)
		{
			if (inventoryContents.contains(matName))
			{
				hasSome = true;
				if (inventoryContents.at(matName) < quant)
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

	registry.emplace_or_replace<component::MyCraftableItems>(entity, craftables, partialCraftables);

}
