#pragma once
#include <string>
#include "Snapshot/Reflection.h"

struct WeightComponent
{
	float value = 1.0f;

private:
	friend class ComponentMetaBinder;
	static inline const std::string_view NAME = "weight";
	static void bind()
	{
		using namespace entt::literals;
		snapshot::reflectComponent<WeightComponent, NAME>()
			.prop("serialize"_hs)
			.data<&WeightComponent::value>("value"_hs);
	}
};

namespace cereal
{
	template<class Archive>
	void serialize(Archive& archive, WeightComponent& weight)
	{
		archive(weight.value);
	}
}
