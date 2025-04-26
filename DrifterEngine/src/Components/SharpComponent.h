#pragma once

#include <string>
#include "Snapshot/Reflection.h"


struct SharpComponent
{
	float sharpness = 0.f;

private:
	friend class ComponentMetaBinder;
	static inline const std::string_view NAME = "Sharp";
	static void bind()
	{
		using namespace entt::literals;
		snapshot::reflectComponent<SharpComponent, NAME>()
			.prop("serialize"_hs)
			.data<&SharpComponent::sharpness>("sharpness"_hs);
	}
};

namespace cereal
{
	template<class Archive>
	void serialize(Archive& archive, SharpComponent& sharp)
	{
		archive(sharp.sharpness);
	}
}








