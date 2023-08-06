#include "pch.h"
#include "GetCurrentCamera.h"
#include "Components/Components.h"
#include "Spatial/Conversions.h"

drft::system::CameraInfo drft::system::getCurrentCamera(entt::registry& registry)
{
	CameraInfo result;
	auto cameraView = registry.view<component::Camera, component::Position>();
	for (auto [_, camera, pos] : cameraView.each())
	{
		result.position = pos.position;
		result.viewport = camera.viewport;
		result.smoothingVector = camera.smoothingVector;
	}
	return result;
}

sf::Vector2f drft::system::toScreenSpace(sf::Vector2i tilePosition, CameraInfo camera)
{
	return (spatial::toWorldSpace(tilePosition - camera.position) - camera.smoothingVector) - sf::Vector2f(camera.viewport.left, camera.viewport.top);
}
