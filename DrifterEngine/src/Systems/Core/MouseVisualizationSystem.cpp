#include "pch.h"
#include "MouseVisualizationSystem.h"
#include <Spatial/Helpers.h>
#include <Spatial/TilePosition.h>
#include <Spatial/WorldGrid.h>
#include <Systems/Helpers/GetMouseTilePosition.h>
#include <Components/RenderComponent.h>
#include <Components/MouseCursorComponent.h>
#include <Components/PathNavComponent.h>
#include <Components/PhysicalBlockingComponent.h> 
#include <Components/PositionComponent.h>
#include <Systems/Helpers/SpawnEffect.h>
#include <Systems/Helpers/GetPlayerHandle.h>
#include <Engine/ControlsContext.h>

using namespace entt::literals;

static const float MaxPathVisualizationDistance = 100.f;

static const SpriteOptions CursorSprite = {
		.uvCoords = sf::Vector2i{0, 0},
		.texture = "mouse_cursor_effect"_hs,
		.uvSize = DefaultTileTextureSize,
		.layer = static_cast<unsigned int>(drft::system::RenderLayer::EffectsFront),
		.color = sf::Color{255, 255, 255, 150}
};
static const SpriteOptions PathSprite = {
		.uvCoords = sf::Vector2i{0, 0},
		.texture = "path_effect"_hs,
		.uvSize = DefaultTileTextureSize,
		.layer = static_cast<unsigned int>(drft::system::RenderLayer::EffectsFront),
		.color = sf::Color{255, 255, 255, 200}
};

void drft::system::MouseVisualizationSystem::init()
{
	
}

void drft::system::MouseVisualizationSystem::start()
{
	_cursor = system::spawnEffect(_registry,
		{
			.frames = { CursorSprite },
			.position = TilePosition{ 0, 0, 0 },
			.ttl = -1,
			.requiresInFOV = false
		});
	_registry.emplace<MouseCursorComponent>(_cursor);
}

void drft::system::MouseVisualizationSystem::update()
{
	auto& controls = _registry.ctx().get<const ControlsContext&>();
	if (controls.navigation == NavigationType::Mouse && !_shouldShowMouse)
	{
		_shouldShowMouse = true;
		showMouse();
	}
	else if (controls.navigation == NavigationType::Keyboard && _shouldShowMouse)
	{
		_shouldShowMouse = false;
		hideMouse();
	}

	updateMousePathVisualization();
}

void drft::system::MouseVisualizationSystem::shutdown()
{
	_registry.destroy(_cursor);
	for (auto&& entity : _visualizedPath)
	{
		_registry.destroy(entity);
	}
	_visualizedPath.clear();
}

void drft::system::MouseVisualizationSystem::showMouse()
{
	auto& render = _registry.get_or_emplace<RenderComponent>(_cursor);
	applySpriteOptionsToRenderComponent(render, CursorSprite);
}

void drft::system::MouseVisualizationSystem::hideMouse()
{
	_registry.remove<RenderComponent>(_cursor);
	for (auto&& entity : _visualizedPath)
	{
		_registry.destroy(entity);
	}
	_visualizedPath.clear();
}

void drft::system::MouseVisualizationSystem::updateMousePathVisualization()
{
	if (!_shouldShowMouse) return;

	auto player = getPlayerConstHandle(_registry);
	if (auto pathNavComponent = player.try_get<PathNavComponent>())
	{
		refreshVisualizedPath(pathNavComponent->path, pathNavComponent->progress);
		return;
	}
	else
	{
		auto mousePosition = _registry.try_get<PositionComponent>(_cursor);
		auto playerPosition = player.try_get<PositionComponent>();

		if (!mousePosition || !playerPosition) return;
		if (mousePosition->tile == _lastMousePosition) return;
		if (spatial::distance3d(mousePosition->tile, playerPosition->tile) > MaxPathVisualizationDistance) return;

		creatNewVisualizedPathToPlayer(mousePosition->tile, playerPosition->tile);
		_lastMousePosition = mousePosition->tile;
	}
}

void drft::system::MouseVisualizationSystem::refreshVisualizedPath(const std::vector<TilePosition>& path, size_t progress)
{
	// TODO: Recycle entities instead of just destroying them all each time
	for (auto&& entity : _visualizedPath)
	{
		_registry.destroy(entity);
	}
	_visualizedPath.clear();

	for (size_t i = progress; i < path.size(); i++)
	{
		auto entity = spawnEffect(_registry,
			{
				.frames = { PathSprite },
				.position = path[i],
				.animationSpeed = 2.0f,
				.ttl = -1,
				.loops = true,
				.requiresInFOV = false,
			});
		_visualizedPath.push_back(entity);
	}
}

void drft::system::MouseVisualizationSystem::creatNewVisualizedPathToPlayer(TilePosition mousePosition, TilePosition playerPosition)
{
	auto& grid = _registry.ctx().get<spatial::WorldGrid>();
	auto path = grid.getPath(playerPosition, mousePosition, 
		[this](const std::vector<entt::entity>& entities) -> int
		{
			for (auto entity : entities)
			{
				if (_registry.all_of<PhysicalBlockingComponent>(entity))
				{
					return 10;
				}
			}
			return 0;
		});
	refreshVisualizedPath(path, 0);
}
