#include "pch.h"
#include "CullingSystem.h"
#include "Components/Components.h"
#include "Components/Tags.h"
#include "Spatial/Conversions.h"

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
			viewport.left - spatial::TILE_WIDTH,
			viewport.top - spatial::TILE_HEIGHT,
			viewport.width + 2*spatial::TILE_WIDTH,
			viewport.height + 2*spatial::TILE_HEIGHT
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

void drft::system::CullingSystem::onFixedUpdateEnd()
{
	registry->clear<component::tag::InViewport>();
}
