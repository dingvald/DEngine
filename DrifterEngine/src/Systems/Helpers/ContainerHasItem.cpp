#include "pch.h"
#include "ContainerHasItem.h"
#include "Components/Components.h"
#include "ItemDatabase.h"

bool drft::system::containerHasItem(entt::registry& registry, entt::entity entity, std::string itemName)
{
    if (auto container = registry.try_get<component::Container>(entity))
    {
        for (auto itemID : container->contents)
        {
            auto itemEntity = ItemDatabase::getEntityFromItemID(itemID);
            auto& prototype = registry.get<component::Prototype>(itemEntity);
            if (prototype.name.compare(itemName) == 0)
            {
                return true;
            }
        }
    }
    
    return false;
}
