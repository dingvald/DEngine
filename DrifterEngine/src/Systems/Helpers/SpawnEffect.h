#pragma once
#include "Systems/Rendering/RenderLayers.h"
#include "Utility/SpriteOptions.h"
#include <Spatial/TilePosition.h>

namespace drft::system
{
	struct EffectStruct
	{
		std::vector<SpriteOptions> frames;

		TilePosition position = { 0,0,0 };
		float animationSpeed = 0.f;
		int ttl = 60; // how many ticks the effect is alive for (60 ticks / second)
		int fadeRate = 0; // decrease in alpha per tick
		bool loops = false;
		bool requiresInFOV = true;
	};

	entt::entity spawnEffect(entt::registry& registry, EffectStruct&& effect);
}