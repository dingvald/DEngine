#pragma once

#include <string>
#include "EnTT/entt.hpp"
#include "Snapshot/Reflection.h"

struct FlyingComponent
{
private:
	bool _placeHolder = true;

private:
	friend class ComponentMetaBinder;
	static inline const std::string_view NAME = "flying";
	static void bind()
	{
		using namespace entt::literals;
		snapshot::reflectComponent<FlyingComponent, NAME>()
			.prop("serialize"_hs);
	}

	friend class cereal::access;
	template<class Archive>
	void serialize(Archive& archive)
	{
		archive(_placeHolder);
	}
};









