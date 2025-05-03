#pragma once

#include <string>
#include "EnTT/entt.hpp"
#include "Snapshot/Reflection.h"
#include "Ability/AbilityType.h"

struct HotbarComponent
{
	std::array<drft::AbilityType, 10> abilities;

private:
	friend class ComponentMetaBinder;
	static inline const std::string_view NAME = "hotbar";
	static void bind()
	{
		using namespace entt::literals;
		snapshot::reflectComponent<HotbarComponent, NAME>()
			.prop("serialize"_hs);
	}
};

namespace cereal
{
	template<class Archive>
	void serialize(Archive& archive, HotbarComponent& hotbar)
	{
		archive(hotbar.abilities);
	}
}

