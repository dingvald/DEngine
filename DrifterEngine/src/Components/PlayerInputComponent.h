#pragma once

#include <string>
#include "EnTT/entt.hpp"
#include "Snapshot/Reflection.h"
#include <Actions/ActionTypeIds.h>

struct PlayerInputComponent
{
	ActionID selectedAction = ActionID::None;

private:
	friend class ComponentMetaBinder;
	static inline const std::string_view NAME = "Player";
	static void bind()
	{
		using namespace entt::literals;
		snapshot::reflectComponent<PlayerInputComponent, NAME>()
			.prop("serialize"_hs);
	}
};

namespace cereal
{
	template<class Archive>
	void serialize(Archive& archive, PlayerInputComponent& player)
	{
		archive(player.selectedAction);
	}
}