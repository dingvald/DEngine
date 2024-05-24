#pragma once

#include <string>
#include "EnTT/entt.hpp"
#include "Snapshot/Reflection.h"

struct PlayerComponent
{
	int sightRange = 10;
private:
	friend class ComponentMetaBinder;
	static inline const std::string_view NAME = "Player";
	static void bind()
	{
		using namespace entt::literals;
		snapshot::reflectComponent<PlayerComponent, NAME>()
			.prop("serialize"_hs)
			.data<&PlayerComponent::sightRange>("sightRange"_hs);
	}
};

namespace cereal
{
	template<class Archive>
	void serialize(Archive& archive, PlayerComponent& player)
	{
		archive(player.sightRange);
	}
}