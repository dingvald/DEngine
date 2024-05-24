#pragma once

#include <string>
#include "EnTT/entt.hpp"
#include "Snapshot/Reflection.h"

struct FactionComponent
{
	std::string name;
private:
	friend class ComponentMetaBinder;
	static inline const std::string_view NAME = "Faction";
	static void bind()
	{
		using namespace entt::literals;
		snapshot::reflectComponent<FactionComponent, NAME>()
			.prop("serialize"_hs)
			.data<&FactionComponent::name>("name"_hs);
	}
};

namespace cereal
{
	template<class Archive>
	void serialize(Archive& archive, FactionComponent& faction) 
	{
		archive(faction.name);
	}
}










