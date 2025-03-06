#pragma once

#include "SFML/System/Vector2.hpp"
#include "SFML/Graphics/Rect.hpp"
#include "EnTT/entt.hpp"


struct CameraComponent
{
	sf::View view;
	float scale = 1.0f;
	sf::Vector3f lag;
	entt::entity target = { entt::null };
};






