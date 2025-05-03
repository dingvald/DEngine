#pragma once

#include <string>
#include "EnTT/entt.hpp"
#include "Snapshot/Reflection.h"
#include "RenderComponent.h"
#include "Utility/SpriteOptions.h"

struct SyncedAnimationComponent
{
	std::vector<SpriteOptions> frames;
	float speed = 1.0f; // animation frames / second
	bool loops = false;

private:
	friend class ComponentMetaBinder;
	static inline const std::string_view NAME = "synced_animation";
	static void bind()
	{
		using namespace entt::literals;
		snapshot::reflectComponent<SyncedAnimationComponent, NAME>()
			.prop("serialize"_hs);
	}
};

namespace cereal
{
	template<class Archive>
	void serialize(Archive& archive, SyncedAnimationComponent& animation)
	{
		archive(
			animation.frames,
			animation.speed,
			animation.loops
		);

	}
}