#pragma once

#include <string>
#include "EnTT/entt.hpp"
#include "Snapshot/Reflection.h"


struct InLiquidComponent
{
	float volume = 0.0f;

private:
	friend class ComponentMetaBinder;
	static inline const std::string_view NAME = "InLiquid";
	static void bind()
	{
		using namespace entt::literals;
		snapshot::reflectComponent<InLiquidComponent, NAME>()
			.prop("serialize"_hs);
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






