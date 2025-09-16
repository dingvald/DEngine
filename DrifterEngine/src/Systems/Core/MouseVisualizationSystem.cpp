#include "pch.h"
#include "MouseVisualizationSystem.h"
#include <Spatial/Helpers.h>
#include <Spatial/TilePosition.h>
#include <Spatial/WorldGrid.h>
#include <Components/RenderComponent.h>
#include <Components/MouseCursorComponent.h>
#include <Components/PathNavComponent.h>
#include <Components/PositionComponent.h>
#include <Systems/Helpers/SpawnEffect.h>
#include <Systems/Helpers/GetPlayerHandle.h>
#include <Engine/ControlsContext.h>
#include <Events/ChangeMouseVisibilityEvent.h>
#include <Spatial/PathingHeuristics/PhysicalBlockingHeuristic.h>

using namespace entt::literals;

static const float MaxPathVisualizationDistance = 100.f;

static const SpriteOptions CursorSprite = {
		.uvCoords = sf::Vector2i{0, 0},
		.texture = "mouse_cursor_effect"_hs,
		.uvSize = DefaultTileTextureSize,
		.layer = drft::RenderLayer::EffectsFront,
		.color = sf::Color{255, 255, 255}
};
static const SpriteOptions PathSprite = {
		.uvCoords = sf::Vector2i{0, 0},
		.texture = "path_effect"_hs,
		.uvSize = DefaultTileTextureSize,
		.layer = drft::RenderLayer::EffectsFront,
		.color = sf::Color{255, 255, 255, 200}
};

namespace Internal
{
	struct RequestVisualizedPathEvent 
	{
		std::vector<drft::TilePosition> path;
	};
	struct ReturnVisualizedPathEvent
	{
		std::vector<drft::TilePosition> path;
	};

	static void onRequestVisualizedPath(drft::system::MouseVisualizationSystem* sys, RequestVisualizedPathEvent* ev)
	{
		if (!sys) return;
		if (!ev) return;
		ev->path = sys->getCachedPath();
	}
}

std::vector<drft::TilePosition> drft::system::MouseVisualizationSystem::getVisualizedPath(entt::registry& registry)
{
	Internal::RequestVisualizedPathEvent ev;
	auto& dispatcher = registry.ctx().get<entt::dispatcher>();
	dispatcher.trigger(&ev);

	return std::move(ev.path);
}

void drft::system::MouseVisualizationSystem::changeMouseVisibility(entt::registry& registry, MouseVisibilityOptions&& options)
{
	events::ChangeMouseVisibilityEvent ev =
	{
		.showPath = options.shouldShowPath,
		.showCursor = options.shouldShowCursor
	};
	registry.ctx().get<entt::dispatcher>().trigger(ev);
}

const std::vector<drft::TilePosition>& drft::system::MouseVisualizationSystem::getCachedPath() const
{
	return _cachedPath;
}

void drft::system::MouseVisualizationSystem::init()
{
	_dispatcher.sink<events::ChangeMouseVisibilityEvent>().connect<&MouseVisualizationSystem::onChangeMouseVisibilityEvent>(this);
	_dispatcher.sink<Internal::RequestVisualizedPathEvent*>().connect<&Internal::onRequestVisualizedPath>(this);
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

void drft::system::MouseVisualizationSystem::fixedUpdate()
{
	auto& controls = _registry.ctx().get<const ControlsContext&>();
	if (controls.isMouseControlled() && !_shouldShowMouse && !_hideMouse)
	{
		showMouse();
	}
	else if (controls.isKeyboardControlled() && _shouldShowMouse)
	{
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
	_shouldShowMouse = true;
}

void drft::system::MouseVisualizationSystem::hideMouse()
{
	_registry.remove<RenderComponent>(_cursor);
	for (auto&& entity : _visualizedPath)
	{
		_registry.destroy(entity);
	}
	_visualizedPath.clear();
	_shouldShowMouse = false;
}

void drft::system::MouseVisualizationSystem::onChangeMouseVisibilityEvent(const events::ChangeMouseVisibilityEvent& ev)
{
	// TODO: add support for show/hide mouse cursor separately
	if (ev.showPath)
	{
		showMouse();
		_hideMouse = false;
	}
	else
	{
		hideMouse();
		_hideMouse = true;
	}
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
	auto tempPath = grid.getPath(playerPosition, mousePosition, spatial::PhysicalBlockingHeuristic{ _registry }, 10);
	
	if (tempPath.empty())
	{
		_cachedPath = {};
	}
	else
	{
		if (tempPath.back() == mousePosition)
		{
			_cachedPath = std::move(tempPath);
		}
		else
		{
			_cachedPath = {};
		}
	}

	refreshVisualizedPath(_cachedPath, 0);
}
