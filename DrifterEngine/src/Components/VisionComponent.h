#pragma once

#include <string>
#include "EnTT/entt.hpp"
#include "Snapshot/Reflection.h"

struct VisionComponent
{
	int sightRange;

private:
	friend class ComponentMetaBinder;
	static inline const std::string_view NAME = "Vision";
	static void bind()
	{
		using namespace entt::literals;
		snapshot::reflectComponent<VisionComponent, NAME>()
			.prop("serialize"_hs)
			.data<&VisionComponent::sightRange>("sight_range"_hs);
	}
};

namespace cereal
{
	template<class Archive>
	void serialize(Archive& archive, VisionComponent& vision)
	{
		archive(vision.sightRange);
	}
}