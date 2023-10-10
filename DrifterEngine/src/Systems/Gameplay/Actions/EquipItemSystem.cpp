#include "pch.h"
#include "EquipItemSystem.h"
#include "Components/Components.h"

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
	auto part = body.parts.search(equipItem.partName);
	if (!part)
	{
		throw std::exception("Trying to equip item to non-existant part name");
	}
	auto currentlyEquipped = part->getEquipped(equipItem.layer);
	auto itemItr = std::find(container.contents.begin(), container.contents.end(), equipItem.toEquip);

	if (itemItr != container.contents.end())
	{
		if (currentlyEquipped.has_value())
		{
			std::swap(*itemItr, currentlyEquipped.value());
		}
		else
		{
			registry.patch<component::Container>(entity, [itemItr](component::Container& cont)
				{
					cont.contents.erase(itemItr);
				});
			
			part->equip(equipItem.toEquip, equipItem.layer);
		}
	}
}

void drft::system::EquipItemSystem::onItemUnequipped(entt::registry& registry, entt::entity entity)
{
	auto& container = registry.get<component::Container>(entity);
	const auto& unequipItem = registry.get<component::action::Unequip>(entity);
	auto& body = registry.get<component::Body>(entity);

	if (auto part = body.parts.search(unequipItem.partName))
	{
		auto optionalItem = part->getEquipped(unequipItem.layer);
		if (optionalItem.has_value())
		{
			unsigned long itemToUnequip = optionalItem.value();
			registry.patch<component::Container>(entity, [itemToUnequip](component::Container& cont)
				{
					cont.contents.push_back(itemToUnequip);
				});
			part->unequip(itemToUnequip);
		}
	}
}
