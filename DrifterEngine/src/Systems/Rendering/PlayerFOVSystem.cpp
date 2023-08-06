#include "pch.h"
#include "PlayerFOVSystem.h"
#include "Components/Components.h"
#include "Components/Tags.h"
#include "Spatial/WorldGrid.h"
#include "Spatial/Helpers.h"
#include "Spatial/Conversions.h"


void drft::system::PlayerFOVSystem::init()
{
	auto& grid = registry->ctx().get<spatial::WorldGrid&>();

	auto blocksLight = [this](sf::Vector2i position) -> bool
	{
		return _lightBlockingPositions.contains(position);
	};
	auto setVisible = [this, &grid](sf::Vector2i position)
	{
		const auto entities = grid.entitiesAt(position);
		_toLight.insert(_toLight.end(), entities.begin(), entities.end());
	};
	auto getDistance = [](sf::Vector2i position) -> int
	{
		return static_cast<int>(spatial::distance({ 0,0 }, position));
	};

	_fov = std::make_unique<Visibility>(blocksLight, setVisible, getDistance);
}

void drft::system::PlayerFOVSystem::fixedUpdate()
{
	auto positions = registry->view<const component::Position, component::tag::InViewport>();
	_lightBlockingPositions.reserve(positions.size_hint());
	for (auto [entity, pos] : positions.each())
	{
		if (registry->any_of<component::LightBlocking>(entity))
		{
			_lightBlockingPositions.emplace(pos.position);
		}
	}

	auto playerView = registry->view<component::Player, component::Position>();
	for (auto [_, player, pos] : playerView.each())
	{
		_fov->compute(pos.position, player.sightRange);
		for (auto entityToLight : _toLight)
		{
			registry->emplace_or_replace<component::tag::InPlayerFOV>(entityToLight);
			if (!registry->all_of<component::Actor>(entityToLight) 
				&& registry->all_of<component::tag::InViewport>(entityToLight))
			{
				registry->emplace_or_replace<component::PlayerHasSeen>(entityToLight);
			}
		}
		_toLight.clear();
	}
	_lightBlockingPositions.clear();
}

void drft::system::PlayerFOVSystem::onFixedUpdateEnd()
{
	registry->clear<component::tag::InPlayerFOV>();
}
