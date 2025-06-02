#include "pch.h"
#include "GetCurrentCamera.h"
#include "Components/CameraComponent.h"
#include "Components/PositionComponent.h"
#include "Spatial/Conversions.h"
#include <Spatial/Helpers.h>
#include <Utility/Math.h>
#include <Utility/Vector3Utils.h>

static CameraComponent EmptyCamera = {};
static PositionComponent EmptyPosition = {};

static constexpr float ZoomIncrement = 2.0f;
static constexpr float MaxZoomScale = 1.0f;
static constexpr float MinZoomScale = 0.0625f;

drft::system::CameraHandle drft::system::getCurrentCamera(entt::registry& registry)
{
	auto cameraView = registry.view<CameraComponent, PositionComponent>();
	for (auto&& [entity, camera, pos] : cameraView.each())
	{
		return CameraHandle {
			.entity = entity, 
			.position = pos, 
			.camera = camera, 
			.isInitialized = (camera.target == entt::null ? false : true) 
		};
	}

	return CameraHandle {
		.entity = entt::null, 
		.position = EmptyPosition, 
		.camera = EmptyCamera, 
		.isInitialized = false 
	};
}

sf::Vector2f drft::system::toScreenSpace(TilePosition tilePosition, CameraHandle camera)
{
	const sf::Vector3f cameraPosition = spatial::toFloatSpace(camera.position.tile) + Vector3Utils::floorToMultiple(camera.camera.lag, camera.camera.scale);
	const sf::Vector3f relativeToCameraPosition = spatial::toFloatSpace(tilePosition) - cameraPosition;

	return spatial::toXY(relativeToCameraPosition);
}

sf::Vector2f drft::system::toScreenSpace(sf::Vector2f worldPosition, CameraHandle camera)
{
	const sf::Vector3f cameraPosition = spatial::toFloatSpace(camera.position.tile) + Vector3Utils::floorToMultiple(camera.camera.lag, camera.camera.scale);
	const sf::Vector2f relativeToCameraPosition = worldPosition - spatial::toXY(cameraPosition);

	return relativeToCameraPosition;
}

drft::TilePosition drft::system::fromScreenSpace(sf::Vector2i screenPosition, CameraHandle camera)
{
	const sf::Vector3f cameraPosition = spatial::toFloatSpace(camera.position.tile) + Vector3Utils::floorToMultiple(camera.camera.lag, camera.camera.scale);
	auto topleft = spatial::toXY(cameraPosition) - (camera.camera.view.getSize() / 2.f);
	auto totalPosition = topleft + sf::Vector2f{ static_cast<float>(screenPosition.x), static_cast<float>(screenPosition.y) };

	return spatial::toTileSpace(sf::Vector3f{ totalPosition.x, totalPosition.y, 0.f }) + TilePosition{ 0,0,camera.position.tile.z };
}

void drft::system::CameraHandle::zoomIn()
{
	if (camera.scale <= MinZoomScale) return;

	camera.view.zoom(1.f / ZoomIncrement);
	camera.scale /= ZoomIncrement;
}

void drft::system::CameraHandle::zoomOut()
{
	if (camera.scale >= MaxZoomScale) return;

	camera.view.zoom(ZoomIncrement);
	camera.scale *= ZoomIncrement;
}

sf::FloatRect drft::system::CameraHandle::getViewRect() const
{
	return { camera.view.getCenter() - camera.view.getSize() / 2.f, camera.view.getSize() };
}
