#include "pch.h"
#include "GetPrimaryMaterial.h"
#include "Components/MaterialComponent.h"
#include "Factory/EntityFactory.h"

std::optional<entt::const_handle> drft::system::getPrimaryMaterial(entt::const_handle entity)
{
    if (auto material = entity.try_get<MaterialComponent>())
    {
        const auto& primaryMaterialName = material->materials.front();
        const auto& factory = entity.registry()->ctx().get<const EntityFactory&>();
        auto primaryMaterial = factory.get(primaryMaterialName);
        return entt::const_handle{ factory.prototypes(), primaryMaterial };
    }
    
    return std::nullopt;
}
