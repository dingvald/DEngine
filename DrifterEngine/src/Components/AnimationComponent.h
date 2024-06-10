#pragma once

#include <string>
#include "EnTT/entt.hpp"
#include "Snapshot/Reflection.h"
#include "RenderComponent.h"
#include "Utility/SpriteOptions.h"

struct AnimationComponent
{
	std::vector<SpriteOptions> sprites;
	int index = 0;
	float speed = 1.0f; // animation frames / second
	float elapsed = 0.0f; // number of game frames elapsed
	bool loops = false;

private:
	friend class ComponentMetaBinder;
	static inline const std::string_view NAME = "Animation";
	static void bind()
	{
		using namespace entt::literals;
		snapshot::reflectComponent<AnimationComponent, NAME>()
			.prop("serialize"_hs);
	}
};

namespace cereal
{
	template<class Archive>
	void serialize(Archive& archive, AnimationComponent& animation)
	{
		archive(
			animation.sprites, 
			animation.index, 
			animation.speed, 
			animation.elapsed, 
			animation.loops
		);

	}
}



