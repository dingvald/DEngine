#pragma once

#include <string>
#include "EnTT/entt.hpp"
#include "Snapshot/Reflection.h"
#include "ItemComponent.h"

struct MyCraftableItemsComponent
{
	std::vector<entt::entity> craftables;
	std::vector<entt::entity> partialCraftables;

private:
	friend class ComponentMetaBinder;
	static inline const std::string_view NAME = "MyCraftableItems";
	static void bind()
	{
		using namespace entt::literals;
		snapshot::reflectComponent<MyCraftableItemsComponent, NAME>()
			.prop("serialize"_hs);
	}
};

namespace cereal
{
	template<class Archive>
	void serialize(Archive& archive, MyCraftableItemsComponent& craftableItems)
	{
		archive(craftableItems.craftables, craftableItems.partialCraftables);
	}
}


