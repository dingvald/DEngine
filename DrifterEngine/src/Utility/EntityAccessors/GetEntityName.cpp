#include "pch.h"
#include "GetEntityName.h"
#include <Components/DescriptionComponent.h>
#include <Utility/StringManipulation.h>

const static std::string MISSING_NAME = "MISSING_NAME";

std::string drft::util::getEntityName(entt::const_handle entity)
{
    if (auto description = entity.try_get<DescriptionComponent>())
    {
        return util::capitalizeAll(description->name);
    }
    return MISSING_NAME;
}
