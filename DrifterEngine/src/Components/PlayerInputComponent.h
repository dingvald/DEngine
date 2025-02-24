#pragma once

#include <string>
#include "EnTT/entt.hpp"
#include "Snapshot/Reflection.h"

struct PlayerInputComponent
{
private:
	std::byte _byte;
	friend class ComponentMetaBinder;
	friend class cereal::access;
	static inline const std::string_view NAME = "Player";
	static void bind()
	{
		using namespace entt::literals;
		snapshot::reflectComponent<PlayerInputComponent, NAME>()
			.prop("serialize"_hs);
	}

	template<class Archive>
	void serialize(Archive& archive)
	{
		archive(_byte);
	}
};