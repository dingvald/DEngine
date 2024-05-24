#pragma once

#include <string>
#include "EnTT/entt.hpp"
#include "Snapshot/Reflection.h"

struct ItemComponent
{
	using ID = unsigned long;
	static const ID NONE = 0;
	ID id = NONE;

private:
	friend class ComponentMetaBinder;
	static inline const std::string_view NAME = "Item";
	static void bind()
	{
		using namespace entt::literals;
		snapshot::reflectComponent<ItemComponent, NAME>()
			.prop("serialize"_hs);
	}
};

namespace cereal
{
	template<class Archive>
	void serialize(Archive& archve, ItemComponent& item)
	{
		archive(item.id);
	}
}










