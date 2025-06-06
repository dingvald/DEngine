#pragma once

#include <string>
#include "EnTT/entt.hpp"
#include "Snapshot/Reflection.h"

struct TickingLifetimeComponent
{
	int ticksRemaining = 0;

private:
	friend class ComponentMetaBinder;
	static inline const std::string_view NAME = "ticking_lifetime";
	static void bind(entt::meta_ctx& ctx)
	{
		using namespace entt::literals;
		snapshot::reflectComponent<TickingLifetimeComponent, NAME>(ctx)
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



