#pragma once
#include "Systems/Rendering/RenderLayers.h"

namespace drft::system
{
	struct EffectStruct
	{
		sf::Color color = sf::Color::Magenta;
		std::vector<unsigned int> sprites = { 4u };
		system::RenderLayer layer = system::RenderLayer::EffectsFront;
		sf::Vector2i position = { 0,0 };
		float animationSpeed = 0.f;
		int ttl = 60; // one second
		bool fades = false;
		bool loops = false;
	};

	entt::entity spawnEffect(entt::registry& registry, EffectStruct&& effect);
}