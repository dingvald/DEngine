#pragma once

#include <string>
#include "EnTT/entt.hpp"
#include "Snapshot/Reflection.h"

struct FactionComponent
{
	std::string name;
private:
	friend class ComponentMetaBinder;
	static inline const std::string_view NAME = "faction";
	static void bind(entt::meta_ctx& ctx)
	{
		using namespace entt::literals;
		snapshot::reflectComponent<FactionComponent, NAME>(ctx)
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










