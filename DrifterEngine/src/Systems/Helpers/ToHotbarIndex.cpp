#include "pch.h"
#include "ToHotbarIndex.h"

size_t drft::system::fromHotbarIndex(int index)
{
    if (index < 0 || index > HOTBAR_SIZE) throw std::exception("Index outside bounds.");
    if (index == 0) index = HOTBAR_SIZE;
    size_t newIndex = index - 1;
    return newIndex;
}

int drft::system::toHotbarIndex(size_t index)
{
    int result = index + 1;
    if (result >= HOTBAR_SIZE) result = 0;

    return result;
}
