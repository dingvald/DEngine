#include "pch.h"
#include "GetEntityWeight.h"
#include <Components/WeightComponent.h>

float drft::util::getEntityWeight(entt::const_handle entity)
{
    if (auto weight = entity.try_get<WeightComponent>())
    {
        return weight->value;
    }
    return 0.0f;
}
