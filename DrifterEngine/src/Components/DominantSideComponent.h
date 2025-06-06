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

	static void bind(entt::meta_ctx& ctx)
	{
		using namespace entt::literals;
		snapshot::reflectComponent<DominantSideComponent, NAME>(ctx)
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