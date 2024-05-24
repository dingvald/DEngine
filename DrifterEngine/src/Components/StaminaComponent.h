#pragma once

#include <string>
#include "EnTT/entt.hpp"
#include "Snapshot/Reflection.h"


struct StaminaComponent
{
	float max = 10.f;
	float current = 10.f;
	float baseConsumption = 0.f;

private:
	friend class ComponentMetaBinder;
	static inline const std::string_view NAME = "Stamina";
	static void bind()
	{
		using namespace entt::literals;
		snapshot::reflectComponent<StaminaComponent, NAME>()
			.prop("serialize"_hs)
			.data<&StaminaComponent::max>("max"_hs)
			.data<&StaminaComponent::baseConsumption>("baseConsumption"_hs);
	}
};

namespace cereal
{
	template<class Archive>
	void serialize(Archive& archive, StaminaComponent& stamina)
	{
		archive(stamina.max, stamina.current, stamina.baseConsumption);
	}
}






