#include "pch.h"
#include "GetMouseTilePosition.h"
#include <Systems/Helpers/GetCurrentCamera.h>

drft::TilePosition drft::getMouseTilePosition(entt::registry& registry)
{
	auto& window = registry.ctx().get<sf::RenderWindow&>();
	auto windowRelativeMousePosition = sf::Mouse::getPosition(window);
	auto camera = system::getCurrentCamera(registry);

	return fromScreenSpace(windowRelativeMousePosition, camera);
}
