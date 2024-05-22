#include "pch.h"
#include "CullingSystem.h"
#include "Components/Components.h"
#include "Components/Tags.h"
#include "Systems/Helpers/GetCurrentCamera.h"
#include "Spatial/Conversions.h"

void drft::system::CullingSystem::init()
{
}

void drft::system::CullingSystem::fixedUpdate()
{
	auto camera = getCurrentCamera(*_registry);
	sf::FloatRect viewport = camera.viewport;
	// Add buffer to viewport
	viewport.left -= spatial::TILE_WIDTH;
	viewport.top -= spatial::TILE_HEIGHT;
	viewport.width += 2 * spatial::TILE_WIDTH;
	viewport.height += 2 * spatial::TILE_HEIGHT;
	
	auto view = _registry->view<component::Position>();
	for (auto [entity, pos] : view.each())
	{
		if (viewport.contains(spatial::toFloatSpace(pos.position - camera.position)))
		{
			_registry->emplace<component::tag::InViewport>(entity);
		}
	}
}

void drft::system::CullingSystem::onFixedUpdateEnd()
{
	_registry->clear<component::tag::InViewport>();
}
