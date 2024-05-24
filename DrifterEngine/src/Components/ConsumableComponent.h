#pragma once

#include <string>
#include "EnTT/entt.hpp"
#include "Snapshot/Reflection.h"
#include "ItemComponent.h"

struct ConsumableComponent
{
	bool destroy = true;

private:
	friend class ComponentMetaBinder;
	static inline const std::string_view NAME = "Consumable";
	static void bind()
	{
		using namespace entt::literals;
		snapshot::reflectComponent<ConsumableComponent, NAME>()
			.prop("serialize"_hs)
			.data<&ConsumableComponent::destroy>("destroy"_hs);
	}
};

namespace cereal
{
	template<class Archive>
	void serialize(Archive& archive, ConsumableComponent& consumable)
	{
		archive(consumable.destroy);
	}
}

