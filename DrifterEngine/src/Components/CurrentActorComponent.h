#pragma once
#include <string>
#include "EnTT/entt.hpp"
#include "Snapshot/Reflection.h"

enum class CurrentActorState
{
	Pending,
	InProgress,
	Complete
};

struct CurrentActorComponent
{
	CurrentActorState state = CurrentActorState::Pending;
	int pointsSpent = 0;
	int ticks = 0;

private:
	friend class ComponentMetaBinder;
	static inline const std::string_view NAME = "CurrentActor";
	static void bind()
	{
		using namespace entt::literals;
		snapshot::reflectComponent<CurrentActorComponent, NAME>()
			.prop("serialize"_hs);
	}
};

namespace cereal
{
	template<class Archive>
	void serialize(Archive& archive, CurrentActorComponent& actor)
	{
		archive(actor.state, actor.pointsSpent);
	}
}