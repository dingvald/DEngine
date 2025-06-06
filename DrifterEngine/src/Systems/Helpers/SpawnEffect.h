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
		sf::Vector3f offset = { 0,0,0 };
		float animationSpeed = 0.f;
		int ttl = 60; // how many ticks the effect is alive for (60 ticks / second)
		int fadeRate = 0; // decrease in alpha per tick
		bool loops = false; // does this effect loop (if its an animation)
		bool requiresInFOV = true; // does the effect need to be in the player's fov to be visible?
	};

	entt::entity spawnEffect(entt::registry& registry, EffectStruct&& effect);
}