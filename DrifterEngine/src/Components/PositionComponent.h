#pragma once

#include "SFML/System/Vector2.hpp"
#include <string>
#include "EnTT/entt.hpp"
#include "Snapshot/Reflection.h"

struct PositionComponent
{
	sf::Vector2i position = { 0,0 };

private:
	friend class ComponentMetaBinder;
	static inline const std::string_view NAME = "Position";
	static void bind()
	{
		using namespace entt::literals;
		snapshot::reflectComponent<PositionComponent, NAME>()
			.prop("serialize"_hs)
			.data<&PositionComponent::position>("position"_hs);
	}
};

namespace cereal
{
	template<class Archive>
	void serialize(Archive& archive, PositionComponent& position)
	{
		archive(position.position.x, position.position.y);
	}
}
