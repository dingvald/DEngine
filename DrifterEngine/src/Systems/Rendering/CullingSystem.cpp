#include "pch.h"
#include "CullingSystem.h"
#include "Components/PositionComponent.h"
#include "Components/Tags.h"
#include "Systems/Helpers/GetCurrentCamera.h"
#include "Spatial/Conversions.h"
#include <Spatial/Helpers.h>


void drft::system::CullingSystem::render(sf::RenderTarget& target)
{
	_registry.clear<component::tag::InViewport>();

	const auto camera = getCurrentCamera(_registry);
	const auto cameraTilePosition = camera.position.tile;

	sf::FloatRect viewRect = camera.getViewRect();
	viewRect = addBufferToViewport(viewRect);

	const auto view = _registry.view<const PositionComponent>();
	for (auto [entity, pos] : view.each())
	{
		if (cameraTilePosition.z != pos.tile.z) continue;

		auto screenPosition = toScreenSpace(pos.tile, camera);
		if (!viewRect.contains(screenPosition)) continue;

		_registry.emplace<component::tag::InViewport>(entity);
	}
}

sf::FloatRect drft::system::CullingSystem::addBufferToViewport(const sf::FloatRect& viewport) const
{
	sf::FloatRect result = viewport;
	result.position.x -= TileDimensions.x;
	result.position.y -= TileDimensions.y;
	result.size.x += 2 * TileDimensions.x;
	result.size.y += 2 * TileDimensions.y;

	return result;
}
