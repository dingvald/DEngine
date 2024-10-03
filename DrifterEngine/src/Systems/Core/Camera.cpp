#include "pch.h"
#include "Camera.h"

#include "Components/CameraComponent.h"
#include "Components/PositionComponent.h"
#include "Components/PlayerComponent.h"

#include <Spatial/Conversions.h>
#include <Spatial/Helpers.h>
#include "Services/DebugInfo.h"

static const float CAMERA_SPEED = 7.0f;


void drft::system::Camera::onStart()
{
	_camera = _registry.create();
	const float viewportWidth = _registry.ctx().get<const sf::RenderWindow&>().getView().getSize().x;
	const float viewportHeight = _registry.ctx().get<const sf::RenderWindow&>().getView().getSize().y;
	// component order matters for camera because it determines order of component destruction.
	_registry.emplace<CameraComponent>(_camera, sf::FloatRect(-viewportWidth / 2, -viewportHeight / 2, viewportWidth, viewportHeight), sf::Vector3f{}, entt::null);
	_registry.emplace<PositionComponent>(_camera, TilePosition{0,0,0});
}

void drft::system::Camera::onUpdateEnd()
{
	auto cameraView = _registry.view<CameraComponent, PositionComponent>();

	for (auto [entity, camera, pos] : cameraView.each())
	{
		if (camera.target == entt::null)
		{
			auto playerView = _registry.view<PlayerComponent, PositionComponent>();
			for (auto [entity, _, playerPos] : playerView.each())
			{
				camera.target = entity;
				pos.tile = playerPos.tile;
			}
		}
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
