#pragma once


namespace drft
{
	class EntityFactory;
	struct TilePosition;
}

namespace drft::gen
{
	entt::handle placeSingle(entt::id_type id, TilePosition position, entt::registry& registry, const EntityFactory& factory);
}