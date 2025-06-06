#pragma once

#include <string>
#include "EnTT/entt.hpp"
#include "Snapshot/Reflection.h"


struct PhysicalBlockingComponent
{
private:
	std::byte _unused;
	friend class ComponentMetaBinder;
	friend class cereal::access;
	static inline const std::string_view NAME = "physical_blocking";
	static void bind(entt::meta_ctx& ctx)
	{
		using namespace entt::literals;
		snapshot::reflectComponent<PhysicalBlockingComponent, NAME>(ctx);
	}

	template<class Archive>
	void serialize(Archive& archive)
	{
		archive(_unused);
	}
};

