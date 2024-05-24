#pragma once

#include <string>
#include "EnTT/entt.hpp"
#include "Snapshot/Reflection.h"

struct CraftableComponent
{
	std::unordered_map<std::string, unsigned long> recipe;

private:
	friend class ComponentMetaBinder;
	static inline const std::string_view NAME = "Craftable";
	static void bind()
	{
		using namespace entt::literals;
		snapshot::reflectComponent<CraftableComponent, NAME>()
			.prop("serialize"_hs)
			.data<&CraftableComponent::recipe>("recipe"_hs);
	}
};

namespace cereal
{
	template<class Archive>
	void serialize(Archive& archive, CraftableComponent& craftable)
	{
		archive(craftable.recipe);
	}
}

