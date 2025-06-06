#pragma once

#include <string>
#include "EnTT/entt.hpp"
#include "Snapshot/Reflection.h"

struct VisualEffectComponent
{
	int ttl = 0;
	int fadeRate = 0;
	bool requiresInFOV = true;

private:
	friend class ComponentMetaBinder;
	static inline const std::string_view NAME = "visual_effect";
	static void bind(entt::meta_ctx& ctx)
	{
		snapshot::reflectComponent<VisualEffectComponent, NAME>(ctx);
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


