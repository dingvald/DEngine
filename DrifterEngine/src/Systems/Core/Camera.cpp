#include "pch.h"
#include "Camera.h"
#include "Components/Components.h"
#include "Spatial/Conversions.h"

static const float CAMERA_SPEED = 5.0f;

void drft::system::Camera::init()
{
}

void drft::system::Camera::update(const float dt)
{
	auto cameraView = registry->view<component::Camera, component::Position>(entt::exclude<component::Prototype>);

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
	}
}
