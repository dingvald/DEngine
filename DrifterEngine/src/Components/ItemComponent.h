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
	static inline const std::string_view NAME = "item";
	static void bind(entt::meta_ctx& ctx)
	{
		using namespace entt::literals;
		snapshot::reflectComponent<ItemComponent, NAME>(ctx);
	}
};

namespace cereal
{
	template<class Archive>
	void serialize(Archive& archive, ItemComponent& item)
	{
		archive(item.id);
	}
}










