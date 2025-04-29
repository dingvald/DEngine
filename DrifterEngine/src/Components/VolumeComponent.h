#pragma once
#include <string>
#include "Snapshot/Reflection.h"

struct VolumeComponent
{
	float value = 1.0f;

private:
	friend class ComponentMetaBinder;
	static inline const std::string_view NAME = "volume";
	static void bind()
	{
		using namespace entt::literals;
		snapshot::reflectComponent<VolumeComponent, NAME>()
			.prop("serialize"_hs)
			.data<&VolumeComponent::value>("value"_hs);
	}
};

namespace cereal
{
	template<class Archive>
	void serialize(Archive& archive, VolumeComponent& weight)
	{
		archive(weight.value);
	}
}