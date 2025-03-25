#include "pch.h"
#include "GetPlayerHandle.h"
#include <Components/PlayerInputComponent.h>
#include <Utility/StandardLogger.h>

entt::handle drft::getPlayerHandle(entt::registry& registry)
{
    auto playerView = registry.view<PlayerInputComponent>();
    if (playerView.size() > 1)
    {
        throw std::exception("Too many players in registry");
    }
    if (playerView.empty())
    {
        LOG_ERROR("could not create player handle");
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
        LOG_ERROR("could not create player handle");
        return entt::const_handle{ registry, entt::null };
    }
    return entt::const_handle{ registry, *playerView.begin() };
}
