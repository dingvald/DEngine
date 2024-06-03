#pragma once

#include <string>
#include "EnTT/entt.hpp"
#include "Snapshot/Reflection.h"


struct AttackerComponent
{
	int baseDamage = 1;

private:
	friend class ComponentMetaBinder;
	static inline const std::string_view NAME = "Attacker";
	static void bind()
	{
		using namespace entt::literals;
		snapshot::reflectComponent<AttackerComponent, NAME>()
			.prop("serialize"_hs)
			.data<&AttackerComponent::baseDamage>("baseDamage"_hs);
	}
};

namespace cereal
{
	template<class Archive>
	void serialize(Archive& archive, AttackerComponent& attacker)
	{
		archive(attacker.baseDamage);
	}
}







