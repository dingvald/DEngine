#pragma once
namespace drft::system
{
	struct EffectStruct
	{
		sf::Color color = sf::Color::Magenta;
		std::vector<unsigned int> sprites = { 4u };
		sf::Vector2i position = { 0,0 };
		float animationSpeed = 0.f;
		int ttl = 60; // one second
		bool fades = false;
		bool loops = false;
	};

	void spawnEffect(entt::registry& registry, EffectStruct&& effect);
}