#pragma once

#include <SFML/System/Vector2.hpp>
#include <string>
#include "EnTT/entt.hpp"
#include "Snapshot/Reflection.h"


struct SprintingComponent
{
	float multiplier = 1.f;
	float staminaCost = 0.f;
private:
	friend class ComponentMetaBinder;
	static inline const std::string_view NAME = "sprinting";
	static void bind(entt::meta_ctx& ctx)
	{
		using namespace entt::literals;
		snapshot::reflectComponent<SprintingComponent, NAME>(ctx);
	}
};

namespace cereal
{
	template<class Archive>
	void serialize(Archive& archive, SprintingComponent& sprinting) 
	{
		archive(
			sprinting.multiplier, 
			sprinting.staminaCost
		);
	}
}










