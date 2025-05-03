#pragma once
#include <string>
#include "EnTT/entt.hpp"
#include "Snapshot/Reflection.h"


struct PrototypeComponent
{
	std::string name = { "PROTOTYPE" };

private:
	friend class ComponentMetaBinder;
	static inline const std::string_view NAME = "prototype";
	static void bind()
	{
		using namespace entt::literals;
		snapshot::reflectComponent<PrototypeComponent, NAME>()
			.prop("serialize"_hs);
	}
};

namespace cereal
{
	template<class Archive>
	void serialize(Archive& archive, PrototypeComponent& prototype)
	{
		archive(prototype.name);
	}
}

