#include "pch.h"
#include "GetEntityMaterials.h"
#include "Components/MaterialComponent.h"
#include "Factory/EntityFactory.h"

std::vector<entt::const_handle> drft::util::getEntityMaterials(entt::const_handle entity)
{
    std::vector<entt::const_handle> result;
    if (auto materialComp = entity.try_get<MaterialComponent>())
    {
        for (auto&& materialName : materialComp->materials)
        {
            const auto& factory = entity.registry()->ctx().get<const EntityFactory&>();
            auto material = factory.get(materialName);
            result.push_back(material);
        }
    }

    return result;
}

std::optional<entt::const_handle> drft::util::getEntityPrimaryMaterial(entt::const_handle entity)
{
    if (auto material = entity.try_get<MaterialComponent>())
    {
        const auto& primaryMaterialName = material->materials.front();
        const auto& factory = entity.registry()->ctx().get<const EntityFactory&>();
        return factory.get(primaryMaterialName);
    }
    
    return std::nullopt;
}
