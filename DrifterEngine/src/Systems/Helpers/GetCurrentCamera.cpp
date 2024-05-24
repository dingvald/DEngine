#include "pch.h"
#include "GetCurrentCamera.h"
#include "Components/CameraComponent.h"
#include "Components/PositionComponent.h"
#include "Spatial/Conversions.h"

drft::system::CameraInfo drft::system::getCurrentCamera(const entt::registry& registry)
{
	CameraInfo result;
	auto cameraView = registry.view<CameraComponent, PositionComponent>();
	for (auto [_, camera, pos] : cameraView.each())
	{
		result.position = pos.position;
		result.viewport = camera.viewport;
		result.lag = camera.lag;
	}
	return result;
}

sf::Vector2f drft::system::toScreenSpace(sf::Vector2i tilePosition, CameraInfo camera)
{
	return (spatial::toFloatSpace(tilePosition - camera.position) - camera.lag) - sf::Vector2f(camera.viewport.left, camera.viewport.top);
}
