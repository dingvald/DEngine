#include <pch.h>
#include "StructureInstanceLayer.h"

StructureInstanceLayer::StructureInstanceLayer(SlotPositionMap&& slots)
    : _slots(std::move(slots))
{
}

const SlotPositionMap& StructureInstanceLayer::getEntitySlots() const
{
    return _slots;
}
