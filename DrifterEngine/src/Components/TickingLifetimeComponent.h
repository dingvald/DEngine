#pragma once

#include <string>
#include "EnTT/entt.hpp"
#include "Snapshot/Reflection.h"

struct TickingLifetimeComponent
{
	int ticksRemaining = 0;

private:
	friend class ComponentMetaBinder;
	static inline const std::string_view NAME = "TickingLifetime";
	static void bind()
	{
		using namespace entt::literals;
		snapshot::reflectComponent<TickingLifetimeComponent, NAME>()
			.prop("serialize"_hs)
			.data<&TickingLifetimeComponent::ticksRemaining>("ticks"_hs);
	}
};

namespace cereal
{
	template<class Archive>
	void serialize(Archive& archive, TickingLifetimeComponent& tickingLifetime)
	{
		archive(tickingLifetime.ticksRemaining);
	}
}



