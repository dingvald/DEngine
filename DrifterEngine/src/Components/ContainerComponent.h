#pragma once

#include <string>
#include "EnTT/entt.hpp"
#include "Snapshot/Reflection.h"
#include "ItemComponent.h"

struct ContainerComponent
{
	std::vector<ItemComponent::ID> contents;
	unsigned int capacity = 0;

private:
	friend class ComponentMetaBinder;
	static inline const std::string_view NAME = "Container";
	static void bind()
	{
		using namespace entt::literals;
		snapshot::reflectComponent<ContainerComponent, NAME>()
			.prop("serialize"_hs)
			.data<&ContainerComponent::capacity>("capacity"_hs);
	}
};

namespace cereal
{
	template<class Archive>
	void serialize(Archive& archive, ContainerComponent& container)
	{
		archive(container.contents, container.capacity);
	}
}
