#include "pch.h"
#include "ItemDatabase.h"
#include "Systems/Core/ItemUniqueIDGenerator.h"

entt::entity ItemDatabase::getEntityFromItemID(unsigned long itemID)
{
	if (drft::system::ItemUniqueIDGenerator::_itemToEntityCache.contains(itemID))
	{
		return drft::system::ItemUniqueIDGenerator::_itemToEntityCache.at(itemID);
	}
	return entt::null;
}
