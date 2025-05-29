#pragma once

#include "Snapshot/Reflection.h"


struct SharpComponent
{
private:
	std::byte _byte;
	friend class ComponentMetaBinder;
	static inline const std::string_view NAME = "sharp";
	static void bind()
	{
		using namespace entt::literals;
		snapshot::reflectComponent<SharpComponent, NAME>()
			.prop("serialize"_hs);
	}
	friend class cereal::access;
	template<class Archive>
	void serialize(Archive& archive)
	{
		archive(_byte);
	}
};









