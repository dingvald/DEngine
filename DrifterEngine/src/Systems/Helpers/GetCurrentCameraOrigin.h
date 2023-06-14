#pragma once

namespace drft::system
{
	// Assumes only one entity has the camera component
	sf::Vector2f getCurrentCameraOrigin(entt::registry& registry);
}