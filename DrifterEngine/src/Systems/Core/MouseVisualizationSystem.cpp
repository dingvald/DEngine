#include "pch.h"
#include "MouseVisualizationSystem.h"
#include <Spatial/TilePosition.h>
#include <Systems/Helpers/GetMouseTilePosition.h>
#include <Components/RenderComponent.h>
#include <Components/MouseCursorComponent.h>
#include <Components/PositionComponent.h>
#include <Systems/Helpers/SpawnEffect.h>
#include <Engine/ControlsContext.h>

using namespace entt::literals;

void drft::system::MouseVisualizationSystem::start()
{
	_cursorSprite = {
		.uvCoords = sf::Vector2i{0, 0},
		.texture = "mouse_cursor_effect"_hs,
		.uvSize = DefaultTileTextureSize,
		.layer = static_cast<unsigned int>(system::RenderLayer::Tiles),
		.color = sf::Color{255, 255, 255, 200} 
	};

	_cursor = system::spawnEffect(_registry,
		{
			.frames = {_cursorSprite},
			.position = TilePosition{0,0,0},
			.ttl = -1,
			.requiresInFOV = false
		});
	_registry.emplace<MouseCursorComponent>(_cursor);
}

void drft::system::MouseVisualizationSystem::update()
{
	auto& controls = _registry.ctx().get<const ControlsContext&>();
	if (controls.navigation == NavigationType::Keyboard)
	{
		_registry.remove<RenderComponent>(_cursor);
		return;
	}
	else
	{
		auto& render = _registry.get_or_emplace<RenderComponent>(_cursor);
		applySpriteOptionsToRenderComponent(render, _cursorSprite);
	}
}

void drft::system::MouseVisualizationSystem::shutdown()
{
	_registry.destroy(_cursor);
}
