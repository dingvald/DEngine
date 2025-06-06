#pragma once

#include <Spatial/TilePosition.h>
#include <string>
#include "EnTT/entt.hpp"
#include "Snapshot/Reflection.h"

struct PositionComponent
{
	drft::TilePosition tile = { 0,0,0 };
	sf::Vector3f offset = { 0,0,0 };

private:
	friend class ComponentMetaBinder;
	static inline const std::string_view NAME = "position";
	static void bind(entt::meta_ctx& ctx)
	{
		using namespace entt::literals;
		snapshot::reflectComponent<PositionComponent, NAME>(ctx)
			.data<&PositionComponent::tile>("position"_hs);
	}
};

namespace cereal
{
	template<class Archive>
	void serialize(Archive& archive, PositionComponent& position)
	{
		archive(
			position.tile.x, 
			position.tile.y, 
			position.tile.z,
			position.offset.x, 
			position.offset.y, 
			position.offset.z
		);
	}
}
