#include "pch.h"
#include "EquipItemSystem.h"

#include "Components/Components.h"
#include "Components/ContainerComponent.h"
#include "Components/BodyComponent.h"

#include "Systems/Helpers/ItemDatabase.h"

void drft::system::EquipItemSystem::init()
{
	_registry->on_construct<component::action::Equip>().connect<&EquipItemSystem::onItemEquipped>(this);
	_registry->on_update<component::action::Equip>().connect<&EquipItemSystem::onItemEquipped>(this);

	_registry->on_construct<component::action::Unequip>().connect<&EquipItemSystem::onItemUnequipped>(this);
	_registry->on_update<component::action::Unequip>().connect<&EquipItemSystem::onItemUnequipped>(this);
}

void drft::system::EquipItemSystem::onUpdateEnd()
{
	_registry->clear<component::action::Equip>();
	_registry->clear<component::action::Unequip>();
}

void drft::system::EquipItemSystem::onItemEquipped(entt::registry& registry, entt::entity entity)
{
	auto& container = registry.get<ContainerComponent>(entity);
	const auto& equipItem = registry.get<component::action::Equip>(entity);
	auto& body = registry.get<BodyComponent>(entity);

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
			registry.patch<ContainerComponent>(entity, [itemItr](ContainerComponent& cont)
				{
					cont.contents.erase(itemItr);
				});
		}
		body.parts.equipItem(equipItem.toEquip, equipItem.layer, equipItem.partName);
	}
}

void drft::system::EquipItemSystem::onItemUnequipped(entt::registry& registry, entt::entity entity)
{
	auto& container = registry.get<ContainerComponent>(entity);
	const auto& unequipItem = registry.get<component::action::Unequip>(entity);
	auto& body = registry.get<BodyComponent>(entity);

	if (container.contents.size() >= container.capacity) return;
	registry.patch<ContainerComponent>(entity, [&unequipItem](ContainerComponent& cont)
		{
			cont.contents.push_back(unequipItem.toUnequip);
		});
	body.parts.unequipItem(unequipItem.toUnequip);
}
