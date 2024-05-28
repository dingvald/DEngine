#include "pch.h"
#include "ContainerHasItem.h"
#include "Components/ContainerComponent.h"
#include "Components/PrototypeComponent.h"
#include "ItemDatabase.h"

bool drft::system::containerHasItem(entt::registry& registry, entt::entity entity, std::string itemName)
{
    if (auto container = registry.try_get<ContainerComponent>(entity))
    {
        for (auto itemID : container->contents)
        {
            auto itemEntity = ItemDatabase::getEntityFromItemID(itemID);
            auto& prototype = registry.get<PrototypeComponent>(itemEntity);
            if (prototype.name == itemName)
            {
                return true;
            }
        }
    }
    
    return false;
}
