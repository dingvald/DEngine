#pragma once

#include <string>
#include "Snapshot/Reflection.h"

struct PlayerHasSeenComponent
{
private:
	std::byte _byte;
	friend class ComponentMetaBinder;
	friend class cereal::access;
	static inline const std::string_view NAME = "player_has_seen";
	static void bind(entt::meta_ctx& ctx)
	{
		using namespace entt::literals;
		snapshot::reflectComponent<PlayerHasSeenComponent, NAME>(ctx);
	}

	template<class Archive>
	void serialize(Archive& archive)
	{
		archive(_byte);
	}
};