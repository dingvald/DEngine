#include "pch.h"
#include "Camera.h"
#include "Components/Components.h"
#include "Spatial/Conversions.h"
#include "Spatial/Helpers.h"
#include "Services/DebugInfo.h"

static const float CAMERA_SPEED = 7.0f;

void drft::system::Camera::init()
{
	
}

void drft::system::Camera::onStart()
{
	_camera = registry->create();
	int viewportWidth = registry->ctx().get<const sf::RenderWindow&>().getView().getSize().x;
	int viewportHeight = registry->ctx().get<const sf::RenderWindow&>().getView().getSize().y;
	// component order matters for camera because it determines order of component destruction.
	registry->emplace<component::Camera>(_camera, sf::FloatRect(0, 0, viewportWidth, viewportHeight), entt::null);
	registry->emplace<component::Position>(_camera, sf::Vector2f(0, 0));
}

void drft::system::Camera::update(const float dt)
{
	auto cameraView = registry->view<component::Camera, component::Position>();

	for (auto [entity, camera, pos] : cameraView.each())
	{
		if (camera.target == entt::null)
		{
			auto playerView = registry->view<component::Player, component::Position>();
			for (auto [entity, _, playerPos] : playerView.each())
			{
				camera.target = entity;
				pos.position = playerPos.position;
			}
		}
	}

	for (auto [entity, camera, pos] : cameraView.each())
	{
		const auto& target = registry->try_get<const component::Position>(camera.target);

		if (!target) continue;


		pos.position.x = std::lerp(pos.position.x, target->position.x, std::clamp(CAMERA_SPEED * dt, 0.f, 1.f));
		pos.position.y = std::lerp(pos.position.y, target->position.y, std::clamp(CAMERA_SPEED * dt, 0.f, 1.f));

		if (spatial::distance(pos.position, target->position) < 0.5)
		{
			pos.position = target->position;
		}

		camera.viewport.left = pos.position.x - (camera.viewport.width / 2);
		camera.viewport.top = pos.position.y - (camera.viewport.height / 2);

		service::DebugInfo::instance().putInfo("Position", std::to_string(target->position.x) + ", " + std::to_string(target->position.y));
	}
}

void drft::system::Camera::shutdown()
{
	registry->destroy(_camera);
}
