#pragma once
#include "EnTT/entt.hpp"
#include "Snapshot/Reflection.h"
#include <Components/ItemComponent.h>

struct ItemIDTrackerComponent
{
	ItemComponent::ID maxID = 1u; // Count starts at 1 for a new game

private:
	friend class ComponentMetaBinder;
	static inline const std::string_view NAME = "MaxItemID";
	static void bind()
	{
		using namespace entt::literals;
		snapshot::reflectComponent<ItemIDTrackerComponent, NAME>()
			.prop("serialize"_hs);
	}
};

namespace cereal
{
	template<class Archive>
	void serialize(Archive& archive, ItemIDTrackerComponent& maxItemID)
	{
		archive(maxItemID.maxID);
	}
}