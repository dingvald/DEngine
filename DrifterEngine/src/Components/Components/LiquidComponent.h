#pragma once

#include <string>
#include "EnTT/entt.hpp"
#include "Snapshot/Reflection.h"


struct LiquidComponent
{
	float volume = 0.0f; // Litres

private:
	friend class ComponentMetaBinder;
	static inline const std::string_view NAME = "Liquid";
	static void bind()
	{
		using namespace entt::literals;
		snapshot::reflectComponent<LiquidComponent, NAME>()
			.prop("serialize"_hs)
			.data<&LiquidComponent::volume>("volume"_hs);
	}
};

namespace cereal
{
	template<class Archive>
	void serialize(Archive& archive, LiquidComponent& liquid)
	{
		archive(liquid.volume);
	}
}





