#include "pch.h"
#include "CullingSystem.h"
#include "Components/PositionComponent.h"
#include "Components/Tags.h"
#include "Systems/Helpers/GetCurrentCamera.h"

namespace Internal
{
	static sf::FloatRect addBufferToViewport(const sf::FloatRect& viewport)
	{
		sf::FloatRect result = viewport;
		result.position.x -= drft::TileDimensions.x;
		result.position.y -= drft::TileDimensions.y;
		result.size.x += 2 * drft::TileDimensions.x;
		result.size.y += 2 * drft::TileDimensions.y;

		return result;
	}
}

void drft::system::CullingSystem::render(sf::RenderTarget& target)
{
	_registry.clear<component::tag::InViewport>();

	const auto camera = getCurrentCamera(_registry);

	sf::FloatRect viewRect = camera.getViewRect();
	viewRect = Internal::addBufferToViewport(viewRect);

	const auto view = _registry.view<const PositionComponent>();
	for (auto [entity, pos] : view.each())
	{
		if (camera.position.tile.z != pos.tile.z) continue;

		auto screenPosition = toScreenSpace(pos.tile, camera);
		if (!viewRect.contains(screenPosition)) continue;

		_registry.emplace<component::tag::InViewport>(entity);
	}
}
