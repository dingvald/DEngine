#include "pch.h"
#include "EquipItemSystem.h"
#include "Components/Components.h"
#include "Systems/Helpers/ItemDatabase.h"

void drft::system::EquipItemSystem::init()
{
	registry->on_construct<component::action::Equip>().connect<&EquipItemSystem::onItemEquipped>(this);
	registry->on_update<component::action::Equip>().connect<&EquipItemSystem::onItemEquipped>(this);

	registry->on_construct<component::action::Unequip>().connect<&EquipItemSystem::onItemUnequipped>(this);
	registry->on_update<component::action::Unequip>().connect<&EquipItemSystem::onItemUnequipped>(this);
}

void drft::system::EquipItemSystem::onUpdateEnd()
{
	registry->clear<component::action::Equip>();
	registry->clear<component::action::Unequip>();
}

void drft::system::EquipItemSystem::onItemEquipped(entt::registry& registry, entt::entity entity)
{
	auto& container = registry.get<component::Container>(entity);
	const auto& equipItem = registry.get<component::action::Equip>(entity);
	auto& body = registry.get<component::Body>(entity);

	auto itemItr = std::find(container.contents.begin(), container.contents.end(), equipItem.toEquip);
	if (itemItr != container.contents.end())
	{
		auto itemEntity = ItemDatabase::getEntityFromItemID(equipItem.toEquip);
		if (auto currentlyEquipped = body.parts.getEquippedOnPart(equipItem.partName, equipItem.layer))
		{
			*itemItr = currentlyEquipped.value();
			body.parts.unequipItem(currentlyEquipped.value());
		}
		else
		{
			registry.patch<component::Container>(entity, [itemItr](component::Container& cont)
				{
					cont.contents.erase(itemItr);
				});
		}
		body.parts.equipItem(equipItem.toEquip, equipItem.layer, equipItem.partName);
	}
}

void drft::system::EquipItemSystem::onItemUnequipped(entt::registry& registry, entt::entity entity)
{
	auto& container = registry.get<component::Container>(entity);
	const auto& unequipItem = registry.get<component::action::Unequip>(entity);
	auto& body = registry.get<component::Body>(entity);

	if (container.contents.size() >= container.capacity) return;
	registry.patch<component::Container>(entity, [&unequipItem](component::Container& cont)
		{
			cont.contents.push_back(unequipItem.toUnequip);
		});
	body.parts.unequipItem(unequipItem.toUnequip);
}
