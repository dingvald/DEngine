#include "pch.h"
#include "PlaceEntities.h"
#include "Factory/EntityFactory.h"
#include "Components/PositionComponent.h"
#include <Spatial/Conversions.h>

entt::handle drft::gen::placeSingle(entt::id_type id, TilePosition position, entt::registry& registry, const EntityFactory& factory)
{
	auto handle = factory.build(id, registry);
	handle.patch<PositionComponent>([position](PositionComponent& pos) {
			pos.tile = position;
			});
	return handle;
}
