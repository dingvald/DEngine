#pragma once
#include "EnTT/entt.hpp"
#include "Snapshot/Reflection.h"
#include <Components/ItemComponent.h>

struct ItemIDTrackerComponent
{
	ItemComponent::ID maxID = 1u; // Count starts at 1 for a new game

private:
	friend class ComponentMetaBinder;
	static inline const std::string_view NAME = "item_id_tracker";
	static void bind(entt::meta_ctx& ctx)
	{
		snapshot::reflectComponent<ItemIDTrackerComponent, NAME>(ctx);
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