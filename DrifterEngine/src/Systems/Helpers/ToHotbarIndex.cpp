#include "pch.h"
#include "ToHotbarIndex.h"
#include <Components/HotbarComponent.h>

size_t drft::system::fromHotbarIndex(int index)
{
    if (index < 0 || index > HotbarComponent::MAX_SIZE) throw std::exception("Index outside bounds.");
    if (index == 0) index = HotbarComponent::MAX_SIZE;
    size_t newIndex = index - 1;
    return newIndex;
}

int drft::system::toHotbarIndex(size_t index)
{
    int result = index + 1;
    if (result >= HotbarComponent::MAX_SIZE) result = 0;

    return result;
}
