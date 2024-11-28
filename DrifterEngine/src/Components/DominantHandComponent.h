#pragma once
#include <string>
#include "EnTT/entt.hpp"
#include "Snapshot/Reflection.h"

struct DominantHandComponent
{
	int slotUID = 0;

private:
	friend class ComponentMetaBinder;
	static inline const std::string_view NAME = "DominantHand";

	static void bind()
	{
		using namespace entt::literals;
		snapshot::reflectComponent<DominantHandComponent, NAME>()
			.prop("serialize"_hs)
			.data<&DominantHandComponent::slotUID>("held_uid"_hs);
	}
};

namespace cereal
{
	template<class Archive>
	void serialize(Archive& archive, DominantHandComponent& dominantHand)
	{
		archive(dominantHand.slotUID);
	}
}