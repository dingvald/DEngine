#include "pch.h"
#include "BodyWrapper.h"
#include <Systems/Helpers/ItemDatabase.h>
#include <Components/WearableComponent.h>

bool drft::BodyWrapper::canEquip(const std::string& slotName, entt::entity item) const
{
    if (!isValid()) return false;

    auto slot = _component->parts.getSlot(slotName);
    if (!slot) return false;

    if (slot->type == BodyPart::Slot::Type::Held)
    {
        return true;
    }

    if (auto wearable = _registry->try_get<WearableComponent>(item))
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

    auto itemID = ItemDatabase::getItemIDFromEntity({ *_registry, item });
    if (itemID == 0ul) return false;

    return _component->parts.equipItem(itemID, slot);
}

bool drft::BodyWrapper::unequip(const std::string& slot)
{
    if (!isValid()) return false;

    auto val = _component->parts.unequipItem(slot);

    return val == 0u ? false : true;
}

entt::entity drft::BodyWrapper::swap(const std::string& slot, entt::entity item)
{
    if (!isValid()) return entt::null;

    auto oldItem = _component->parts.unequipItem(slot);
    this->equip(slot, item);

    return ItemDatabase::getEntityFromItemID(oldItem);
}

bool drft::BodyWrapper::hasSlot(const std::string& slot) const
{
    if (!isValid()) return false;

    auto slotPtr = _component->parts.getSlot(slot);

    return slotPtr != nullptr;
}

BodyPart::Slot* drft::BodyWrapper::getSlot(const std::string& slot)
{
    if (!isValid()) return nullptr;

    return _component->parts.getSlot(slot);
}
