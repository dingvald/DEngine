#pragma once

#include <unordered_set>
#include <string>
#include "EnTT/entt.hpp"
#include "Snapshot/Reflection.h"


struct InheritanceComponent
{
	std::unordered_set<std::string> bases;

private:
	friend class ComponentMetaBinder;
	static inline const std::string_view NAME = "Inheritance";
	static void bind()
	{
		using namespace entt::literals;
		snapshot::reflectComponent<InheritanceComponent, NAME>()
			.prop("serialize"_hs);
	}
};

namespace cereal
{
	template<class Archive>
	void serialize(Archive& archive, InheritanceComponent& inheritance)
	{
		archive(inheritance.bases);
	}
}
