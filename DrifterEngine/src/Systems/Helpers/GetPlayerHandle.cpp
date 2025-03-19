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
    if (playerView.empty())
    {
        return entt::handle{ registry, entt::null };
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
    if (playerView.empty())
    {
        return entt::const_handle{ registry, entt::null };
    }
    return entt::const_handle{ registry, *playerView.begin() };
}
