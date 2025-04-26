#include "pch.h"
#include "GetEntityMaterials.h"
#include "Components/MaterialCompositionComponent.h"
#include "Factory/EntityFactory.h"

std::vector<entt::const_handle> drft::util::getEntityMaterials(entt::const_handle entity)
{
    std::vector<entt::const_handle> result;
    if (auto materialComp = entity.try_get<MaterialCompositionComponent>())
    {
        const auto& factory = entity.registry()->ctx().get<const EntityFactory&>();
        for (auto&& [name, percent] : materialComp->materials)
        {
            auto material = factory.get(name);
            result.push_back(material);
        }
    }

    return result;
}

std::optional<entt::const_handle> drft::util::getEntityPrimaryMaterial(entt::const_handle entity)
{
    float largestPercent = 0.f;
    std::string largestName = {};
    if (auto material = entity.try_get<MaterialCompositionComponent>())
    {
        for (auto&& [name, percent] : material->materials)
        {
            if (percent > largestPercent)
            {
                largestPercent = percent;
                largestName = name;
            }
        }
        const auto& factory = entity.registry()->ctx().get<const EntityFactory&>();
        auto handle = factory.get(largestName);
        if (!handle) return std::nullopt;
        return handle;
    }
    
    return std::nullopt;
}

std::vector<std::pair<entt::const_handle, float>> drft::util::getEntityMaterialPercentages(entt::const_handle entity)
{
    std::vector<std::pair<entt::const_handle, float>> result;

    const auto& factory = entity.registry()->ctx().get<const EntityFactory&>();
    if (auto* composition = entity.try_get<MaterialCompositionComponent>())
    {
        for (auto&& [name, percentage] : composition->materials)
        {
            auto handle = factory.get(name);
            if (!handle) continue;
            result.emplace_back(handle, percentage);
        }
    }
    return result;
}
