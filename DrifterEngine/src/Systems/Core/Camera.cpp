#include "pch.h"
#include "Camera.h"
#include "Components/Components.h"
#include "Spatial/Conversions.h"
#include "Services/DebugInfo.h"

static const float CAMERA_SPEED = 5.0f;

void drft::system::Camera::init()
{
	auto camera = registry->create();
	int viewportWidth = registry->ctx().get<sf::Window>().getSize().x;
	int viewportHeight = registry->ctx().get<sf::Window>().getSize().y;
	registry->emplace<component::Position>(camera, sf::Vector2f(0,0), (int)spatial::Layer::Camera);
	registry->emplace<component::Camera>(camera, sf::FloatRect(0, 0, viewportWidth, viewportHeight), entt::null);
}

void drft::system::Camera::update(const float dt)
{
	auto cameraView = registry->view<component::Camera, component::Position>(entt::exclude<component::Prototype>);

	for (auto [entity, camera, pos] : cameraView.each())
	{
		if (camera.target == entt::null)
		{
			auto playerView = registry->view<component::Player, component::Position>(entt::exclude<component::Prototype>);
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

		if (std::abs(pos.position.x - target->position.x) < 0.5f)
		{
			pos.position.x = target->position.x;
		}
		else
		{
			pos.position.x = std::lerp(pos.position.x, target->position.x, CAMERA_SPEED * dt);
		}
		if (std::abs(pos.position.y - target->position.y) < 0.5f)
		{
			pos.position.y = target->position.y;
		}
		else
		{
			pos.position.y = std::lerp(pos.position.y, target->position.y, CAMERA_SPEED * dt);
		}

		camera.viewport.left = pos.position.x - (camera.viewport.width / 2);
		camera.viewport.top = pos.position.y - (camera.viewport.height / 2);

		service::DebugInfo::instance().putInfo("Position", std::to_string(target->position.x) + ", " + std::to_string(target->position.y));
	}
}

void drft::system::Camera::shutdown()
{
	auto cameraView = registry->view<component::Camera, component::Position>(entt::exclude<component::Prototype>);
	for (auto [entity, camera, pos] : cameraView.each())
	{
		camera.target = entt::null;
	}
}
