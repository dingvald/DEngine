#pragma once

#include "SFML/System/Vector2.hpp"
#include "SFML/Graphics/Rect.hpp"
#include "EnTT/entt.hpp"


struct CameraComponent
{
	sf::FloatRect viewport;
	sf::Vector3f lag;
	entt::entity target = { entt::null };
};






