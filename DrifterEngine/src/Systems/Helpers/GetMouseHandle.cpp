#include "pch.h"
#include "GetMouseHandle.h"

#include <Components/MouseCursorComponent.h>

entt::handle drft::getMouseHandle(entt::registry& registry)
{
    auto mouseView = registry.view<MouseCursorComponent>();
    if (mouseView.size() > 1)
    {
        throw std::exception("Too many mice in registry");
    }
    return entt::handle{ registry, *mouseView.begin() };
}

entt::const_handle drft::getMouseConstHandle(entt::registry& registry)
{
    auto mouseView = registry.view<MouseCursorComponent>();
    if (mouseView.size() > 1)
    {
        throw std::exception("Too many mice in registry");
    }
    return entt::const_handle{ registry, *mouseView.begin() };
}
