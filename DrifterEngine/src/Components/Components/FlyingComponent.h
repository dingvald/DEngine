#pragma once

#include <string>
#include "EnTT/entt.hpp"
#include "Snapshot/Reflection.h"

struct FlyingComponent
{
private:
	friend class ComponentMetaBinder;
	static inline const std::string_view NAME = "Flying";
	static void bind()
	{
		using namespace entt::literals;
		snapshot::reflectComponent<FlyingComponent, NAME>()
			.prop("serialize"_hs);
	}
};

namespace cereal
{
	template<class Archive>
	void serialize(Archive&, FlyingComponent&) 
	{

	}
}









