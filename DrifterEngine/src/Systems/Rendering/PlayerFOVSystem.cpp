#include "pch.h"
#include "PlayerFOVSystem.h"
#include "Components/Components.h"
#include "Components/PlayerInputComponent.h"
#include "Components/ActorComponent.h"
#include "Components/PositionComponent.h"
#include "Components/LightBlockingComponent.h"
#include <Components/VisionComponent.h>
#include "Components/Tags.h"
#include "Spatial/WorldGrid.h"
#include "Spatial/Helpers.h"
#include "Spatial/Conversions.h"


void drft::system::PlayerFOVSystem::init()
{
	auto& grid = _registry.ctx().get<spatial::WorldGrid&>();

	auto blocksLight = [this](sf::Vector3i position) -> bool
	{
		return _lightBlockingPositions.contains(position);
	};
	auto setVisible = [this, &grid](sf::Vector3i position)
	{
		const auto& entities = grid.entitiesAt(spatial::asTileSpace(position));
		_toLight.insert(_toLight.end(), entities.begin(), entities.end());
	};
	auto getDistance = [](sf::Vector3i position) -> int
	{
		return static_cast<int>(spatial::distance3d({ 0,0,0 }, position));
	};

	_fov = std::make_unique<Visibility>(blocksLight, setVisible, getDistance);
}

void drft::system::PlayerFOVSystem::render(sf::RenderTarget& target)
{
	_registry.clear<component::tag::InPlayerFOV>();

	auto lightBlockingPositions = _registry.view<const PositionComponent, LightBlockingComponent>();
	_lightBlockingPositions.reserve(lightBlockingPositions.size_hint());
	for (auto&& [entity, pos, lightBlocking] : lightBlockingPositions.each())
	{
		_lightBlockingPositions.emplace(pos.tile);
	}

	auto playerView = _registry.view<PlayerInputComponent, VisionComponent, PositionComponent>();
	for (auto&& [entity, player, vision, pos] : playerView.each())
	{
		_fov->compute(pos.tile, vision.sightRange);
	}

	for (auto&& entity : _toLight)
	{
		_registry.emplace_or_replace<component::tag::InPlayerFOV>(entity);
		_registry.emplace_or_replace<component::tag::PlayerHasSeen>(entity);
	}

	_toLight.clear();
	_lightBlockingPositions.clear();
}
