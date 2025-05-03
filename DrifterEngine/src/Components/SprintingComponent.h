#pragma once

#include <SFML/System/Vector2.hpp>
#include <string>
#include "EnTT/entt.hpp"
#include "Snapshot/Reflection.h"


struct SprintingComponent
{
	sf::Vector2i direction = { 0,0 };
private:
	friend class ComponentMetaBinder;
	static inline const std::string_view NAME = "sprinting";
	static void bind()
	{
		using namespace entt::literals;
		snapshot::reflectComponent<SprintingComponent, NAME>()
			.prop("serialize"_hs);
	}
};

namespace cereal
{
	template<class Archive>
	void serialize(Archive& archive, SprintingComponent& sprinting) 
	{
		archive(sprinting.direction.x, sprinting.direction.y);
	}
}










