#include "pch.h"
#include "GetPrimaryMaterial.h"
#include "Components/Components.h"
#include "Factory/EntityFactory.h"

entt::const_handle drft::system::getPrimaryMaterial(entt::const_handle entity)
{
    if (auto physical = entity.try_get<component::Physical>())
    {
        const auto& primaryMaterialName = physical->materials.front();
        const auto& factory = entity.registry()->ctx().get<const EntityFactory&>();
        auto primaryMaterial = factory.get(primaryMaterialName);
        return { factory.prototypes(), primaryMaterial };
    }
    
    return entt::const_handle();
}
