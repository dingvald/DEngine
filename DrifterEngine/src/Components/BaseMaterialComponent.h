#pragma once
#include <string>
#include "Snapshot/Reflection.h"

struct BaseMaterialComponent
{
private:
	std::byte _byte;
	friend class ComponentMetaBinder;
	
	static inline const std::string_view NAME = "base_material";
	static void bind()
	{
		using namespace entt::literals;
		snapshot::reflectComponent<BaseMaterialComponent, NAME>()
			.prop("serialize"_hs);
	}

	friend class cereal::access;
	template<class Archive>
	void serialize(Archive& archive)
	{
		archive(_byte);
	}
};