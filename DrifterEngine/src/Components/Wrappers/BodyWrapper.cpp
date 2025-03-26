#include "pch.h"
#include "BodyWrapper.h"
#include <Systems/Helpers/ItemDatabase.h>
#include <Components/WearableComponent.h>

bool drft::BodyWrapper::canEquip(const std::string& slotName, entt::entity item) const
{
    if (!isValid()) return false;

    auto slot = tryGetUnderlyingConst()->parts.getSlot(slotName);
    if (!slot) return false;

    if (slot->type == BodyPart::Slot::Type::Held)
    {
        return true;
    }

    if (auto wearable = tryGetRegistry()->try_get<WearableComponent>(item))
    {
        for (auto&& slotName : wearable->slots)
        {
            if (slot->type == BodyPart::stringToSlotType(slotName))
            {
                return true;
            }
        }
    }

    return false;
}

bool drft::BodyWrapper::equip(const std::string& slot, entt::entity item)
{
    if (!isValid()) return false;

    auto itemID = ItemDatabase::getItemIDFromEntity({ *tryGetRegistry(), item});
    if (itemID == 0u) return false;

    bool result = false;
    modify([itemID, &slot, &result](auto& comp) {result = comp.parts.equipItem(itemID, slot);});

    return result;
}

bool drft::BodyWrapper::unequip(const std::string& slot)
{
    if (!isValid()) return false;

    unsigned long itemRemoved = 0;
    modify([&slot, &itemRemoved](auto& comp) {itemRemoved = comp.parts.unequipItem(slot);});

    return itemRemoved == 0u ? false : true;
}

entt::entity drft::BodyWrapper::swap(const std::string& slot, entt::entity item)
{
    if (!isValid()) return entt::null;

    unsigned long itemRemoved = 0;
    modify([&slot, &itemRemoved](auto& comp) {itemRemoved = comp.parts.unequipItem(slot);});
    this->equip(slot, item);

    return ItemDatabase::getEntityFromItemID(itemRemoved);
}

bool drft::BodyWrapper::hasSlot(const std::string& slot) const
{
    if (!isValid()) return false;

    auto slotPtr = tryGetUnderlyingConst()->parts.getSlot(slot);

    return slotPtr != nullptr;
}

const BodyPart::Slot* drft::BodyWrapper::getSlot(const std::string& slot) const
{
    if (!isValid()) return nullptr;

    return tryGetUnderlyingConst()->parts.getSlot(slot);
}
