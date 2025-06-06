#pragma once

#include <string>
#include "EnTT/entt.hpp"
#include "Snapshot/Reflection.h"


struct HealthComponent
{
	float max = std::numeric_limits<float>::min();
	float current = std::numeric_limits<float>::min();
	float recovery = 0.f;

private:
	friend class ComponentMetaBinder;
	static inline const std::string_view NAME = "health";
	static void bind(entt::meta_ctx& ctx)
	{
		using namespace entt::literals;
		snapshot::reflectComponent<HealthComponent, NAME>(ctx)
			.data<&HealthComponent::max>("max"_hs)
			.data<&HealthComponent::recovery>("recovery"_hs);
	}
};

namespace cereal
{
	template<class Archive>
	void serialize(Archive& archive, HealthComponent& health)
	{
		archive(health.max, health.current, health.recovery);
	}
}





