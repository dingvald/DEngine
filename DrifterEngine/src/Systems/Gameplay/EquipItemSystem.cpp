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

	auto& currentlyEquipped = body.parts.at(equipItem.slotname);
	auto itemItr = std::find(container.contents.begin(), container.contents.end(), equipItem.toEquip);

	if (itemItr != container.contents.end())
	{
		if (currentlyEquipped > 0)
		{
			std::swap(*itemItr, currentlyEquipped);
		}
		else
		{
			registry.patch<component::Container>(entity, [itemItr](component::Container& cont)
				{
					cont.contents.erase(itemItr);
				});
			
			body.parts.at(equipItem.slotname) = equipItem.toEquip;
		}
	}
}

void drft::system::EquipItemSystem::onItemUnequipped(entt::registry& registry, entt::entity entity)
{
	auto& container = registry.get<component::Container>(entity);
	const auto& unequipItem = registry.get<component::action::Unequip>(entity);
	auto& body = registry.get<component::Body>(entity);

	if (body.parts.contains(unequipItem.slotname))
	{
		registry.patch<component::Container>(entity, [&body, &unequipItem](component::Container& cont)
			{
				cont.contents.push_back(body.parts.at(unequipItem.slotname));
			});
		
		body.parts.at(unequipItem.slotname) = 0;
	}
}
