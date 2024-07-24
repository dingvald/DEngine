#include "pch.h"
#include "CullingSystem.h"
#include "Components/PositionComponent.h"
#include "Components/Tags.h"
#include "Systems/Helpers/GetCurrentCamera.h"
#include "Spatial/Conversions.h"


void drft::system::CullingSystem::onFixedUpdate()
{
	const auto camera = getCurrentCamera(_registry);
	const auto viewport = addBufferToViewport(camera.viewport);
	
	const auto view = _registry.view<const PositionComponent>();
	for (auto [entity, pos] : view.each())
	{
		if (viewport.contains(spatial::toFloatSpace(pos.position - camera.position)))
		{
			_registry.emplace<component::tag::InViewport>(entity);
		}
	}
}

void drft::system::CullingSystem::onFixedUpdateEnd()
{
	_registry.clear<component::tag::InViewport>();
}

sf::FloatRect drft::system::CullingSystem::addBufferToViewport(const sf::FloatRect& viewport) const
{
	sf::FloatRect result = viewport;
	result.left -= spatial::TILE_WIDTH;
	result.top -= spatial::TILE_HEIGHT;
	result.width += 2 * spatial::TILE_WIDTH;
	result.height += 2 * spatial::TILE_HEIGHT;

	return result;
}
