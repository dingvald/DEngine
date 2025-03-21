#pragma once
#include <string>
#include "EnTT/entt.hpp"
#include "Snapshot/Reflection.h"

struct DominantSideComponent
{
	std::string side = "right";

private:
	friend class ComponentMetaBinder;
	static inline const std::string_view NAME = "dominant_side";

	static void bind()
	{
		using namespace entt::literals;
		snapshot::reflectComponent<DominantSideComponent, NAME>()
			.prop("serialize"_hs)
			.data<&DominantSideComponent::side>("side"_hs);
	}
};

namespace cereal
{
	template<class Archive>
	void serialize(Archive& archive, DominantSideComponent& dominantHand)
	{
		archive(dominantHand.side);
	}
}