#pragma once

#include <string>
#include "EnTT/entt.hpp"
#include "Snapshot/Reflection.h"
#include "RenderComponent.h"
#include "Utility/SpriteOptions.h"

struct AnimationComponent
{
	std::vector<SpriteOptions> frames;
	float speed = 1.0f; // animation frames / second
	bool loops = false;

	int index = 0;
	float elapsed = 0.0f; // number of game frames elapsed
	
private:
	friend class ComponentMetaBinder;
	static inline const std::string_view NAME = "animation";
	static void bind(entt::meta_ctx& ctx)
	{
		snapshot::reflectComponent<AnimationComponent, NAME>(ctx);
	}
};

namespace cereal
{
	template<class Archive>
	void serialize(Archive& archive, AnimationComponent& animation)
	{
		archive(
			animation.frames,
			animation.speed,
			animation.loops,
			animation.index, 
			animation.elapsed
		);

	}
}



