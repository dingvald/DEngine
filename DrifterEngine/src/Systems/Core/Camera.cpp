#include "pch.h"
#include "Camera.h"

#include "Components/CameraComponent.h"
#include "Components/PositionComponent.h"
#include "Components/CameraTargetComponent.h"

#include <Spatial/Conversions.h>
#include <Spatial/Helpers.h>
#include "Services/DebugInfo.h"

static const float CAMERA_SPEED = 7.0f;

void drft::system::Camera::start()
{
	_camera = _registry.create();

	sf::View view = _registry.ctx().get<const sf::RenderWindow&>().getView();
	view.setCenter({ 0,0 });

	// component order matters for camera because it determines order of component destruction.
	_registry.emplace<CameraComponent>(_camera, view, sf::Vector3f{}, entt::null);
	_registry.emplace<PositionComponent>(_camera, TilePosition{0,0,0});
}

void drft::system::Camera::render(sf::RenderTarget& target)
{
	auto cameraView = _registry.view<CameraComponent, PositionComponent>();

	for (auto [entity, camera, pos] : cameraView.each())
	{
		camera.target = tryFindTarget();
	}

	for (auto [entity, camera, pos] : cameraView.each())
	{
		const auto& target = _registry.try_get<const PositionComponent>(camera.target);
		if (!target) continue;

		pos.tile = target->tile;

		service::DebugInfo::instance().putInfo("Position", TilePosition::toString(pos.tile));
	}
}

void drft::system::Camera::shutdown()
{
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
