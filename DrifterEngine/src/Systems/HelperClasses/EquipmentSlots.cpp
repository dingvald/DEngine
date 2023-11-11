#include "pch.h"
#include "EquipmentSlots.h"

std::vector<EquipmentSlot> convertStringsToSlots(const std::vector<std::string>& slots)
{
    std::vector<EquipmentSlot> result;
    for (auto& slot : slots)
    {
        if (!String2EquipmentSlot.contains(slot)) throw std::exception("Invalid slot name");
        result.push_back(String2EquipmentSlot.at(slot));
    }
    return result;
}
