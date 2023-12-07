#include "pch.h"
#include "ToHotbarIndex.h"

int drft::system::toHotbarIndex(int index)
{
    if (index < 0 || index > HOTBAR_SIZE) throw std::exception("Index outside bounds.");
    if (index == 0) index = HOTBAR_SIZE;
    size_t newIndex = index - 1;
    return newIndex;
}
