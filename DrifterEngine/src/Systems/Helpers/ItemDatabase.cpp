#include "pch.h"
#include "ItemDatabase.h"
#include "Components/ItemComponent.h"
#include "Systems/Core/ItemUniqueIDGenerator.h"

entt::entity ItemDatabase::getEntityFromItemID(unsigned long itemID)
{
	if (itemID == ItemComponent::NONE) return entt::null;
	if (drft::system::ItemUniqueIDGenerator::_itemToEntityCache.contains(itemID))
	{
		return drft::system::ItemUniqueIDGenerator::_itemToEntityCache.at(itemID);
	}
	return entt::null;
}
