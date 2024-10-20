#pragma once
#include <Spatial/TilePosition.h>
#include <SFML/System/Vector3.hpp>
#include <Systems/Helpers/EasingFunctions.h>


struct Tween
{
	sf::Vector3f targetOffset;
	float time;
	// Input: value [0.0, 1.0]
	// Output: value [0.0, 1.0]
	// Defaults to linear
	std::function<float(float)> easing = [](float f) {return f;};
	std::function<void(entt::handle)> onFinish = [](entt::handle) {return;}; // Defaults to do nothing
};

struct TweeningComponent
{
	sf::Vector3f offset = {};
	std::vector<Tween> tweens = {};

	uint8_t index = 0;
	float elapsed = 0.f;
};