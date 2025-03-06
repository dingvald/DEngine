#include "pch.h"
#include "GetMouseTilePosition.h"
#include <Systems/Helpers/GetCurrentCamera.h>

drft::TilePosition drft::getMouseTilePosition(entt::registry& registry)
{
	auto& window = registry.ctx().get<sf::RenderWindow&>();
	auto windowRelativeMousePosition = sf::Mouse::getPosition(window);
	auto camera = system::getCurrentCamera(registry);

	sf::Vector2f scaled;
	scaled.x = static_cast<float>(windowRelativeMousePosition.x) * camera.camera.scale;
	scaled.y = static_cast<float>(windowRelativeMousePosition.y) * camera.camera.scale;

	return fromScreenSpace(sf::Vector2i{ scaled }, camera);
}
