#pragma once
#include <Spatial/TilePosition.h>
#include <Components/CameraComponent.h>
#include <Components/PositionComponent.h>

namespace drft::system
{
	struct CameraHandle
	{
		PositionComponent& position;
		CameraComponent& camera;
		bool isInitialized = false;

		void zoomIn();
		void zoomOut();
	};

	// Assumes only one entity has the camera component
	CameraHandle getCurrentCamera(entt::registry& registry);
	sf::Vector2f toScreenSpace(TilePosition tilePosition, CameraHandle camera);
	sf::Vector2f toScreenSpace(sf::Vector2f worldPosition, CameraHandle camera);

	TilePosition fromScreenSpace(sf::Vector2i screenPosition, CameraHandle camera);
}