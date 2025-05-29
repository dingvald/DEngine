#pragma once

#include <string>
#include "EnTT/entt.hpp"
#include "Snapshot/Reflection.h"


struct HealingComponent
{
	float amount = 0.f;
	// if > 1, then it applies the amount over time
	int ticks = 0;

private:
	friend class ComponentMetaBinder;
	static inline const std::string_view NAME = "healing";
	static void bind()
	{
		using namespace entt::literals;
		snapshot::reflectComponent<HealingComponent, NAME>()
			.prop("serialize"_hs)
			.data<&HealingComponent::amount>("amount"_hs)
			.data<&HealingComponent::ticks>("ticks"_hs);
	}
};

namespace cereal
{
	template<class Archive>
	void serialize(Archive& archive, HealingComponent& healing)
	{
		archive(healing.amount, healing.ticks);
	}
}






