#include "pch.h"
#include "GetEntityName.h"
#include <Components/DescriptionComponent.h>
#include <Utility/EntityAccessors/GetEntityMaterials.h>
#include <Utility/StringManipulation.h>
#include <Utility/StandardLogger.h>

namespace InternalOnly
{
    void logMissingComponentWarning(const std::string& variable, const std::string& entityName)
    {
        LOG_WARNING("Can not use variable name {}, entity {} does not have the proper component", variable, entityName);
    }
    void logUnknownComponentVariableWarning(const std::string& variable, const std::string& entityName)
    {
        LOG_WARNING("Variable name {} found in entity name {} can not be handled", variable, entityName);
    }
}

std::string drft::util::getEntityName(entt::const_handle entity)
{
    if (auto description = entity.try_get<DescriptionComponent>())
    {
        std::vector<std::string> splitString = util::split(description->name, " ");
        std::string result;
        for (auto&& str : splitString)
        {
            auto variable = getStringBetween(str, "${", "}");
            if (variable.has_value())
            {
                if (variable == "material")
                {
                    auto material = getEntityPrimaryMaterial(entity);
                    if (!material)
                    {
                        InternalOnly::logMissingComponentWarning(variable.value(), description->name);
                        continue;
                    }
                    result += getEntityName(material.value());
                }
                else
                {
                    InternalOnly::logUnknownComponentVariableWarning(variable.value(), description->name);
                }
            }
            result += str;
        }
        return result;
    }
    return "UNNAMED";
}
