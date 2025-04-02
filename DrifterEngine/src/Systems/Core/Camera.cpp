#include "pch.h"
#include "Camera.h"

#include "Components/CameraComponent.h"
#include "Components/PositionComponent.h"
#include "Components/CameraTargetComponent.h"

#include <Spatial/Helpers.h>

#include <Systems/Helpers/GetCurrentCamera.h>

static const float CAMERA_SPEED = 7.0f;

static const float STARTING_ZOOM_SCALE = 0.5f;

void drft::system::Camera::start()
{
	_camera = _registry.create();

	sf::RenderWindow& window = _registry.ctx().get<sf::RenderWindow>();

	sf::View view = window.getView();
	view.setCenter({ 0,0 });
	view.zoom(STARTING_ZOOM_SCALE);

	// component order matters for camera because it determines order of component destruction.
	_registry.emplace<CameraComponent>(_camera, view, STARTING_ZOOM_SCALE, sf::Vector3f{}, entt::null);
	_registry.emplace<PositionComponent>(_camera, TilePosition{0,0,0});
}

void drft::system::Camera::updateEnd()
{
	CameraHandle camera = getCurrentCamera(_registry);
	camera.camera.target = tryFindTarget();

	const auto target = _registry.try_get<const PositionComponent>(camera.camera.target);
	if (!target) return;

	sf::Vector2f targetScreenPosition = toScreenSpace(target->tile, camera);
	targetScreenPosition.x += TileDimensions.x / 2.f;
	targetScreenPosition.y += TileDimensions.y / 2.f;

	if (camera.getViewRect().contains(targetScreenPosition))
	{
		smoothCameraToTarget(targetScreenPosition, camera);
	}
	else
	{
		snapCameraToTarget(target->tile, camera);
	}
	
}

void drft::system::Camera::render(sf::RenderTarget& target)
{
	
}

void drft::system::Camera::shutdown()
{
	// Prevents saving the camera entity
	_registry.destroy(_camera);
}

void drft::system::Camera::smoothCameraToTarget(sf::Vector2f targetPosition, CameraHandle& cam) const
{
	sf::Vector2f cameraPosition = toScreenSpace(cam.position.tile, cam) + spatial::toXY(cam.camera.lag);

	sf::Vector2f delta = targetPosition - cameraPosition;

	sf::Vector3f moveDistance = sf::Vector3f{ delta.x, delta.y, 0 } * 0.08f;

	cam.camera.lag += moveDistance;

	auto collapsed = spatial::collapseOffset(cam.position.tile, cam.camera.lag);

	cam.position.tile = collapsed.position;
	cam.camera.lag = collapsed.offset;
}

void drft::system::Camera::snapCameraToTarget(TilePosition targetPosition, CameraHandle& cam) const
{
	cam.position.tile = targetPosition;
	cam.camera.lag = { 0, 0, 0 };
}

entt::entity drft::system::Camera::tryFindTarget() const
{
	auto view = _registry.view<CameraTargetComponent, PositionComponent>();
	for (auto entity : view)
	{
		return entity;
	}
	return entt::null;
}
