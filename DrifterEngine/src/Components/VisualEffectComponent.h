#pragma once

#include <string>
#include "EnTT/entt.hpp"
#include "Snapshot/Reflection.h"
#include "Ability/AbilityType.h"

struct VisualEffectComponent
{
	int ttl = 0;
	int fadeRate = 0;
	bool requiresInFOV = true;

private:
	friend class ComponentMetaBinder;
	static inline const std::string_view NAME = "VisualEffect";
	static void bind()
	{
		using namespace entt::literals;
		snapshot::reflectComponent<VisualEffectComponent, NAME>()
			.prop("serialize"_hs);
	}
};

namespace cereal
{
	template<class Archive>
	void serialize(Archive& archive, VisualEffectComponent& visualEffect)
	{
		archive(visualEffect.ttl, visualEffect.fadeRate, visualEffect.requiresInFOV);
	}
}


