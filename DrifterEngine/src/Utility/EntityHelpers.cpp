#include "pch.h"
#include "EntityHelpers.h"
#include "Factory/EntityFactory.h"
#include "Components/PositionComponent.h"
#include "Components/DescriptionComponent.h"
#include <Components/SpriteControllerComponent.h>
#include "Spatial/Helpers.h"

#include <Systems/Rendering/SpriteControllerSystem.h>

using namespace entt::literals;

RenderComponent drft::util::getRenderData(entt::const_handle entity)
{
	return system::SpriteControllerSystem::getDefaultRenderComponent(entity);
}

bool drft::util::isType(entt::const_handle entity, entt::id_type typeId)
{
	if (!entity.registry()->ctx().contains<const EntityFactory&>()) return false;

	const auto& factory = entity.registry()->ctx().get<const EntityFactory&>();

	return factory.getFlattenedInheritance(entity).contains(typeId);
}

float drft::util::getDistanceBetween(entt::const_handle e1, entt::const_handle e2)
{
	if (e1 == e2) return 0.f;

	if (auto pos1 = e1.try_get<PositionComponent>())
	{
		if (auto pos2 = e2.try_get<PositionComponent>())
		{
			return spatial::distance3d(pos1->tile, pos2->tile);
		}
	}
	return std::numeric_limits<float>().infinity();
}
