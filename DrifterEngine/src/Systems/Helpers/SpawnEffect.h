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
		float animationSpeed = 0.f; // frames / second
		std::optional<float> ttl = std::nullopt; // in seconds
		std::function<float(float)> fadeFunc = [](float f) {return f;};
		bool loops = false; // does this effect loop (if its an animation)
		bool requiresInFOV = true; // does the effect need to be in the player's fov to be visible?
	};

	entt::entity spawnEffect(entt::registry& registry, EffectStruct&& effect);
}