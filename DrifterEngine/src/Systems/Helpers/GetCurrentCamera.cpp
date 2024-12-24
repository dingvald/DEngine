#include "pch.h"
#include "GetCurrentCamera.h"
#include "Components/CameraComponent.h"
#include "Components/PositionComponent.h"
#include "Spatial/Conversions.h"
#include <Spatial/Helpers.h>

static CameraComponent EmptyCamera = {};
static PositionComponent EmptyPosition = {};

drft::system::CameraInfo drft::system::getCurrentCamera(const entt::registry& registry)
{
	auto cameraView = registry.view<CameraComponent, PositionComponent>();
	for (auto&& [_, camera, pos] : cameraView.each())
	{
		return CameraInfo{ .position = pos, .camera = camera, .isInitialized = (camera.target == entt::null ? false : true) };
	}

	return CameraInfo{ .position = EmptyPosition, .camera = EmptyCamera, .isInitialized = false };
}

sf::Vector2f drft::system::toScreenSpace(TilePosition tilePosition, CameraInfo camera)
{
	const sf::Vector3f cameraPosition = spatial::toFloatSpace(camera.position.tile) - camera.camera.lag;
	const sf::Vector3f relativeToCameraPosition = spatial::toFloatSpace(tilePosition) - cameraPosition;

	return spatial::toXY(relativeToCameraPosition);
}

sf::Vector2f drft::system::toScreenSpace(sf::Vector2f worldPosition, CameraInfo camera)
{
	const sf::Vector3f cameraPosition = spatial::toFloatSpace(camera.position.tile) - camera.camera.lag;
	const sf::Vector2f relativeToCameraPosition = worldPosition - spatial::toXY(cameraPosition);

	return relativeToCameraPosition;
}

drft::TilePosition drft::system::fromScreenSpace(sf::Vector2i screenPosition, CameraInfo camera)
{
	const sf::Vector3f cameraPosition = spatial::toFloatSpace(camera.position.tile) - camera.camera.lag;
	auto topleft = spatial::toXY(cameraPosition) - (camera.camera.view.getSize() / 2.f);
	auto totalPosition = topleft + sf::Vector2f{ static_cast<float>(screenPosition.x), static_cast<float>(screenPosition.y) };

	return spatial::toTileSpace(sf::Vector3f{ totalPosition.x, totalPosition.y, static_cast<float>(camera.position.tile.z) });
}

