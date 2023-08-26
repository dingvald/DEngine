#pragma once

namespace drft::system
{
	struct CameraInfo
	{
		sf::Vector2i position;
		sf::Vector2f lag;
		sf::FloatRect viewport;
	};

	// Assumes only one entity has the camera component
	CameraInfo getCurrentCamera(entt::registry& registry);

	sf::Vector2f toScreenSpace(sf::Vector2i tilePosition, CameraInfo camera);
}