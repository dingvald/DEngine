#include "pch.h"
#include "GetPlayerHandle.h"
#include <Components/PlayerInputComponent.h>

entt::handle drft::getPlayerHandle(entt::registry& registry)
{
    auto playerView = registry.view<PlayerInputComponent>();
    if (playerView.size() > 1)
    {
        throw std::exception("Too many players in registry");
    }
    return entt::handle{registry, *playerView.begin()};
}

entt::const_handle drft::getPlayerConstHandle(entt::registry& registry)
{
    auto playerView = registry.view<PlayerInputComponent>();
    if (playerView.size() > 1)
    {
        throw std::exception("Too many players in registry");
    }
    return entt::const_handle{ registry, *playerView.begin() };
}
