#pragma once

#include <string>
#include "EnTT/entt.hpp"
#include "Snapshot/Reflection.h"


struct ActorComponent
{
	int ap = 0;
	float moveSpeed = 1.0f;
	float actSpeed = 1.0f;

private:
	friend class ComponentMetaBinder;
	static inline const std::string_view NAME = "Actor";
	static void bind()
	{
		using namespace entt::literals;
		snapshot::reflectComponent<ActorComponent, NAME>()
			.prop("serialize"_hs)
			.data<&ActorComponent::moveSpeed>("moveSpeed"_hs)
			.data<&ActorComponent::actSpeed>("actSpeed"_hs);
	}
};

namespace cereal
{
	template<class Archive>
	void serialize(Archive& archive, ActorComponent& actor)
	{
		archive(actor.ap, actor.moveSpeed, actor.actSpeed);
	}
}




