#include "pch.h"
#include "CullingSystem.h"
#include "Components/Components.h"
#include "Components/Tags.h"

void drft::system::CullingSystem::init()
{
}

void drft::system::CullingSystem::fixedUpdate()
{
	sf::FloatRect viewport;
	sf::FloatRect bufferdViewport;
	const auto camera = registry->view<const component::Camera, const component::Position>();
	// assumes only one camera, otherwise will just use the last one's position
	for (auto const& [entity, cam, pos] : camera.each())
	{
		viewport = cam.viewport;
		bufferdViewport = {
			viewport.left - 32,
			viewport.top - 32,
			viewport.width + 64,
			viewport.height + 64
		};
	}

	auto view = registry->view<component::Position>();
	for (auto [entity, pos] : view.each())
	{
		if (bufferdViewport.contains(pos.position))
		{
			registry->emplace<component::tag::InViewport>(entity);
		}
	}
}
