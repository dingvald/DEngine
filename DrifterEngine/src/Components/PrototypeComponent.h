#pragma once
#include <string>
#include "EnTT/entt.hpp"
#include "Snapshot/Reflection.h"


struct PrototypeComponent
{
	entt::id_type id = entt::hashed_string{ "NONE" };

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
		archive(prototype.id);
	}
}

