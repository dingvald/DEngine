#include "pch.h"
#include "GetCurrentCamera.h"
#include "Components/CameraComponent.h"
#include "Components/PositionComponent.h"
#include "Spatial/Conversions.h"
#include <Spatial/Helpers.h>

drft::system::CameraInfo drft::system::getCurrentCamera(const entt::registry& registry)
{
	CameraInfo result;
	auto cameraView = registry.view<CameraComponent, PositionComponent>();
	for (auto [_, camera, pos] : cameraView.each())
	{
		result.position = pos.tile;
		result.viewport = camera.viewport;
		result.lag = camera.lag;
		if (camera.target != entt::null)
		{
			result.isInitialized = true;
		}
	}

	return result;
}

sf::Vector2f drft::system::toScreenSpace(TilePosition tilePosition, CameraInfo camera)
{
	const sf::Vector3f cameraPosition = spatial::toFloatSpace(spatial::asTileSpace(camera.position)) - camera.lag;
	const sf::Vector3f relativeToCameraPosition = spatial::toFloatSpace(tilePosition) - cameraPosition;

	return spatial::toXY(relativeToCameraPosition) - sf::Vector2f(camera.viewport.left, camera.viewport.top);
}

sf::Vector2f drft::system::toScreenSpace(sf::Vector2f worldPosition, CameraInfo camera)
{
	const sf::Vector3f cameraPosition = spatial::toFloatSpace(spatial::asTileSpace(camera.position)) - camera.lag;
	const sf::Vector2f relativeToCameraPosition = worldPosition - spatial::toXY(cameraPosition);

	return relativeToCameraPosition - sf::Vector2f(camera.viewport.left, camera.viewport.top);
}
