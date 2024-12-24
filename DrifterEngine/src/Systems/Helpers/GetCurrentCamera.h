#pragma once
#include <Spatial/TilePosition.h>
#include <Components/CameraComponent.h>
#include <Components/PositionComponent.h>

namespace drft::system
{
	struct CameraInfo
	{
		const PositionComponent& position;
		const CameraComponent& camera;
		bool isInitialized = false;
	};

	// Assumes only one entity has the camera component
	CameraInfo getCurrentCamera(const entt::registry& registry);

	sf::Vector2f toScreenSpace(TilePosition tilePosition, CameraInfo camera);
	sf::Vector2f toScreenSpace(sf::Vector2f worldPosition, CameraInfo camera);

	TilePosition fromScreenSpace(sf::Vector2i screenPosition, CameraInfo camera);
}