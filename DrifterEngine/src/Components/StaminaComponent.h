#pragma once

#include <string>
#include "EnTT/entt.hpp"
#include "Snapshot/Reflection.h"


struct StaminaComponent
{
	float max = std::numeric_limits<float>::min();
	float current = std::numeric_limits<float>::min();
	float baseConsumption = 0.f;

private:
	friend class ComponentMetaBinder;
	static inline const std::string_view NAME = "stamina";
	static void bind()
	{
		using namespace entt::literals;
		snapshot::reflectComponent<StaminaComponent, NAME>()
			.prop("serialize"_hs)
			.data<&StaminaComponent::baseConsumption>("base_consumption"_hs);
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






