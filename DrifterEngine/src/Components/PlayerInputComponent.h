#pragma once

#include <string>
#include "Snapshot/Reflection.h"

struct PlayerInputComponent
{
private:
	std::byte _byte;
	friend class ComponentMetaBinder;
	friend class cereal::access;
	static inline const std::string_view NAME = "player_input";
	static void bind(entt::meta_ctx& ctx)
	{
		using namespace entt::literals;
		snapshot::reflectComponent<PlayerInputComponent, NAME>(ctx);
	}

	template<class Archive>
	void serialize(Archive& archive)
	{
		archive(_byte);
	}
};