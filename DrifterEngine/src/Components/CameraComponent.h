#pragma once

struct CameraComponent
{
	sf::View view;
	float scale = 1.0f;
	sf::Vector3f lag;
	entt::entity target = { entt::null };
};






