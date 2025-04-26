#include "pch.h"
#include "MaterialPropertiesSystem.h"
#include <Components/MaterialCompositionComponent.h>
#include <Components/MaterialPropertiesComponent.h>
#include <Factory/EntityFactory.h>
#include <Utility/StandardLogger.h>
#include <Utility/EntityAccessors/GetEntityName.h>


void drft::system::MaterialPropertiesSystem::init()
{
	_registry.on_construct<MaterialCompositionComponent>().connect<&MaterialPropertiesSystem::onMaterialCompositionComponentAdded>(this);
	_registry.on_update<MaterialCompositionComponent>().connect<&MaterialPropertiesSystem::onMaterialCompositionComponentUpdated>(this);
}

void drft::system::MaterialPropertiesSystem::onMaterialCompositionComponentAdded(entt::registry& registry, entt::entity entity)
{
	entt::handle handle = { registry, entity };
	const auto& materialComposition = handle.get<MaterialCompositionComponent>();
	auto& materialProperties = handle.emplace_or_replace<MaterialPropertiesComponent>();

	const auto& factory = _registry.ctx().get<EntityFactory>();
	for (auto&& [materialName, percent] : materialComposition.materials)
	{
		auto mat = factory.get(materialName);
		if (!mat)
		{
			auto entityName = util::getEntityName(handle);
			LOG_WARNING("Cannot find material {} for entity {}", materialName, entityName);
			continue;
		}

		if (const auto matProperties = mat.try_get<MaterialPropertiesComponent>())
		{
			materialProperties.hardness += matProperties->hardness * percent;
			materialProperties.brittleness += matProperties->brittleness * percent;
			materialProperties.malleability += matProperties->malleability * percent;
		}
		else
		{
			auto entityName = util::getEntityName(handle);
			LOG_WARNING("Material {} on entity {} does not have MaterialPropertiesComponent", materialName, entityName);
		}
	}
}

void drft::system::MaterialPropertiesSystem::onMaterialCompositionComponentUpdated(entt::registry& registry, entt::entity entity)
{
	onMaterialCompositionComponentAdded(registry, entity);
}
