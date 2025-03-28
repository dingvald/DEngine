#include "pch.h"
#include "GetEntityDescription.h"
#include <Components/DescriptionComponent.h>

static const std::string EmptyDescription = "DESCRIPTION MISSING";

const std::string& drft::util::getEntityDescription(entt::const_handle entity)
{
    if (auto description = entity.try_get<DescriptionComponent>())
    {
        return description->description;
    }
    return EmptyDescription;
}

