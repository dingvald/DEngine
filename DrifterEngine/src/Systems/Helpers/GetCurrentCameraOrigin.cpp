#include "pch.h"
#include "GetCurrentCameraOrigin.h"
#include "Components/Components.h"

sf::Vector2f drft::system::getCurrentCameraOrigin(entt::registry& registry)
{
	sf::Vector2f cameraOrigin;
	auto cameraView = registry.view<component::Camera, component::Position>();
	for (auto [_, camera, pos] : cameraView.each())
	{
		cameraOrigin = sf::Vector2f{ camera.viewport.left, camera.viewport.top };
	}
    return cameraOrigin;
}
