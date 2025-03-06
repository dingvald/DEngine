#include "pch.h"
#include "Camera.h"

#include "Components/CameraComponent.h"
#include "Components/PositionComponent.h"
#include "Components/CameraTargetComponent.h"

#include <Spatial/Conversions.h>
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

void drft::system::Camera::render(sf::RenderTarget& target)
{
	auto cameraView = _registry.view<CameraComponent, PositionComponent>();

	for (auto&& [entity, camera, pos] : cameraView.each())
	{
		camera.target = tryFindTarget();
	}

	for (auto&& [entity, camera, pos] : cameraView.each())
	{
		const auto& target = _registry.try_get<const PositionComponent>(camera.target);
		if (!target) continue;

		pos.tile = target->tile;
	}
}

void drft::system::Camera::shutdown()
{
	// Prevents saving the camera entity
	_registry.destroy(_camera);
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
