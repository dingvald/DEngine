#pragma once

#include <string>
#include "EnTT/entt.hpp"
#include "Snapshot/Reflection.h"


struct LightBlockingComponent
{
	float opacity = 1.0f;

private:
	friend class ComponentMetaBinder;
	static inline const std::string_view NAME = "light_blocking";
	static void bind()
	{
		using namespace entt::literals;
		snapshot::reflectComponent<LightBlockingComponent, NAME>()
			.prop("serialize"_hs)
			.data<&LightBlockingComponent::opacity>("opacity"_hs);
	}
};

namespace cereal
{
	template<class Archive>
	void serialize(Archive& archive, LightBlockingComponent& lightBlocking)
	{
		archive(lightBlocking.opacity);
	}
}





