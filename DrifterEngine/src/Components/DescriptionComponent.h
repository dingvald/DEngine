#pragma once

#include <string>
#include "EnTT/entt.hpp"
#include "Snapshot/Reflection.h"


struct DescriptionComponent
{
	std::string name = { "" };
	std::string description = { "" };

private:
	friend class ComponentMetaBinder;
	static inline const std::string_view NAME = "description";
	static void bind()
	{
		using namespace entt::literals;
		snapshot::reflectComponent<DescriptionComponent, NAME>()
			.prop("serialize"_hs)
			.data<&DescriptionComponent::name>("name"_hs)
			.data<&DescriptionComponent::description>("description"_hs);
	}
};

namespace cereal
{
	template<class Archive>
	void serialize(Archive& archive, DescriptionComponent& description)
	{
		archive(description.name, description.description);
	}
}