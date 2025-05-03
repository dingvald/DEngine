#include "pch.h"
#include "GetEntityPrototype.h"
#include <Components/PrototypeComponent.h>

entt::id_type drft::util::getEntityPrototype(entt::const_handle entity)
{
    if (auto prototype = entity.try_get<PrototypeComponent>())
    {
        return prototype->id;
    }
    return entt::hashed_string{ "NONE" };
}
