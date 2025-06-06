#pragma once

#include "Snapshot/Reflection.h"

struct HotbarComponent
{
	static const int MAX_SIZE = 10;
	std::array<entt::id_type, MAX_SIZE> abilities;

private:
	friend class ComponentMetaBinder;
	static inline const std::string_view NAME = "hotbar";
	static void bind(entt::meta_ctx& ctx)
	{
		snapshot::reflectComponent<HotbarComponent, NAME>(ctx);
	}
};

namespace cereal
{
	template<class Archive>
	void serialize(Archive& archive, HotbarComponent& hotbar)
	{
		archive(hotbar.abilities);
	}
}

