#pragma once
#include "Components/RenderComponent.h"
#include "Systems/Rendering/RenderLayers.h"

namespace drft::system
{
	struct EffectStruct
	{
		std::vector<RenderComponent> sprites;

		sf::Vector2i position = { 0,0 };
		float animationSpeed = 0.f;
		int ttl = 60; // one second
		bool fades = false;
		bool loops = false;
		bool requiresInFOV = true;
	};

	entt::entity spawnEffect(entt::registry& registry, EffectStruct&& effect);
}