#include "pch.h"
#include "GetEntityDescription.h"
#include <Components/DescriptionComponent.h>
#include <Utility/StringManipulation.h>

static const std::string EmptyDescription = "DESCRIPTION MISSING";

std::string drft::util::getEntityDescription(entt::const_handle entity)
{
    if (auto description = entity.try_get<DescriptionComponent>())
    {
        return util::capitalize(description->description);
    }
    return EmptyDescription;
}

