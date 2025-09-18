#include "pch.h"
#include "GetGlobalRandomObject.h"

static drft::rng::Random FallbackRandom = { 0 };

drft::rng::Random& getGlobalRandomObject(entt::registry& registry)
{
    if (!registry.ctx().contains<drft::rng::Random>())
    {
        return FallbackRandom;
    }
    return registry.ctx().get<drft::rng::Random>();
}
