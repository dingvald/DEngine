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
	static void bind()
	{
		using namespace entt::literals;
		snapshot::reflectComponent<PlayerHasSeenComponent, NAME>()
			.prop("serialize"_hs);
	}

	template<class Archive>
	void serialize(Archive& archive)
	{
		archive(_byte);
	}
};