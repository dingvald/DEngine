#include "pch.h"
#include "MaterialCompositionSystem.h"
#include <Components/MaterialCompositionComponent.h>
#include <Components/SolidMaterialComponent.h>
#include <Components/LiquidMaterialComponent.h>
#include <Components/GasMaterialComponent.h>
#include <Factory/EntityFactory.h>
#include <Utility/StandardLogger.h>
#include <Utility/EntityAccessors/GetEntityName.h>
#include <Utility/EntityAccessors/GetEntityMaterials.h>


void drft::system::MaterialCompositionSystem::init()
{
	_registry.on_construct<MaterialCompositionComponent>().connect<&MaterialCompositionSystem::onMaterialCompositionComponentAdded>(this);
	_registry.on_update<MaterialCompositionComponent>().connect<&MaterialCompositionSystem::onMaterialCompositionComponentUpdated>(this);
}

void drft::system::MaterialCompositionSystem::onMaterialCompositionComponentAdded(entt::registry& registry, entt::entity entity)
{
	entt::handle handle = { registry, entity };
	const auto& materialComposition = handle.get<const MaterialCompositionComponent>();
	handle.remove<SolidMaterialComponent>();
	handle.remove<LiquidMaterialComponent>();
	handle.remove<GasMaterialComponent>();

	const auto primaryMaterial = util::getEntityPrimaryMaterial(handle);
	if (!primaryMaterial)
	{
		auto& entityName = util::getEntityName(handle);
		LOG_WARNING("Cannot find primary material for entity {}", entityName);
		return;
	}

	MaterialType type = MaterialType::Solid;
	if (primaryMaterial->all_of<SolidMaterialComponent>()) type = MaterialType::Solid;
	else if (primaryMaterial->all_of<LiquidMaterialComponent>()) type = MaterialType::Liquid;
	else if (primaryMaterial->all_of<GasMaterialComponent>()) type = MaterialType::Gas;

	const auto& factory = _registry.ctx().get<EntityFactory>();
	for (auto&& [materialName, percent] : materialComposition.materials)
	{
		auto mat = factory.get(materialName);
		if (!mat)
		{
			auto& entityName = util::getEntityName(handle);
			LOG_WARNING("Cannot find material {} for entity {}", materialName, entityName);
			continue;
		}

		mergeMaterialProperties(handle, type, mat, percent);
	}
}

void drft::system::MaterialCompositionSystem::onMaterialCompositionComponentUpdated(entt::registry& registry, entt::entity entity)
{
	onMaterialCompositionComponentAdded(registry, entity);
}

void drft::system::MaterialCompositionSystem::mergeMaterialProperties(entt::handle subject, MaterialType primaryType, entt::const_handle other, float percent) const
{
	switch (primaryType)
	{
	case drft::system::MaterialCompositionSystem::MaterialType::Solid:
	{
		auto& solid = subject.get_or_emplace<SolidMaterialComponent>();
		if (auto otherSolid = other.try_get<SolidMaterialComponent>())
		{
			solid.density += otherSolid->density * percent;
			solid.brittleness += otherSolid->brittleness * percent;
			solid.hardness += otherSolid->hardness * percent;
			solid.malleability += otherSolid->malleability * percent;
		}
		else if (auto otherLiquid = other.try_get<LiquidMaterialComponent>())
		{
			solid.density += otherLiquid->density * percent;
			// Add self so it still adds to 100%
			solid.brittleness += solid.brittleness * percent;
			solid.hardness += solid.hardness * percent;
			solid.malleability += solid.malleability * percent;
		}
		else if (auto otherGas = other.try_get<GasMaterialComponent>())
		{
			solid.density += otherGas->density * percent;
			// Add self so it still adds to 100%
			solid.brittleness += solid.brittleness * percent;
			solid.hardness += solid.hardness * percent;
			solid.malleability += solid.malleability * percent;
		}
		break;
	}	
	case drft::system::MaterialCompositionSystem::MaterialType::Liquid:
	{
		auto& liquid = subject.get_or_emplace<LiquidMaterialComponent>();
		if (auto otherLiquid = other.try_get<LiquidMaterialComponent>())
		{
			liquid.density += otherLiquid->density * percent;
			liquid.viscosity += otherLiquid->viscosity * percent;
		}
		break;
	}
	case drft::system::MaterialCompositionSystem::MaterialType::Gas:
	{
		auto& gas = subject.get_or_emplace<GasMaterialComponent>();
		if (auto otherGas = other.try_get<GasMaterialComponent>())
		{
			gas.density += otherGas->density * percent;
		}
		break;
	}
	default:
		break;
	}
}

