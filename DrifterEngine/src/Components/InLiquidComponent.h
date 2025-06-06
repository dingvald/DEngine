#pragma once

#include <string>
#include "EnTT/entt.hpp"
#include "Snapshot/Reflection.h"


struct InLiquidComponent
{
	float volume = 0.0f;

private:
	friend class ComponentMetaBinder;
	static inline const std::string_view NAME = "in_liquid";
	static void bind(entt::meta_ctx& ctx)
	{
		snapshot::reflectComponent<InLiquidComponent, NAME>(ctx);
	}
};

namespace cereal
{
	template<class Archive>
	void serialize(Archive& archive, InLiquidComponent& inLiquid)
	{
		archive(inLiquid.volume);
	}
}






