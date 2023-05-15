#include "pch.h"
#include "LightingSystem.h"
#include "Components/Components.h"
#include "Components/Tags.h"
#include "Spatial/Helpers.h"
#include "Spatial/Conversions.h"
#include "Utility/Visibility.h"
#include "Spatial/WorldGrid.h"

void drft::system::LightingSystem::init()
{
	const auto& grid = registry->ctx().get<const spatial::WorldGrid&>();
	auto blocksLight = [this](sf::Vector2i position) -> bool
	{
		return _lightBlockingPositions.contains(position);
	};
	auto setVisible = [this, &grid](sf::Vector2i position)
	{
		auto entities = grid.entitiesAt(position);
		_toLight.insert(_toLight.end(), entities.begin(), entities.end());
	};
	auto getDistance = [](sf::Vector2i position) -> int
	{
		return static_cast<int>(spatial::distance({0,0}, position));
	};

	_fov = std::make_unique<Visibility>(blocksLight, setVisible, getDistance);
}

void drft::system::LightingSystem::fixedUpdate()
{
	auto lighting = registry->view<const component::LightSource, const component::Position, component::tag::InViewport>();
	auto positions = registry->view<const component::Position, component::tag::InViewport>();
	for (auto [entity, pos] : positions.each())
	{
		if (registry->any_of<component::LightBlocking>(entity))
		{
			_lightBlockingPositions.insert(spatial::toTileSpace(pos.position));
		}
	}
	for (auto [_, light, lightpos] : lighting.each())
	{
		_fov->compute(spatial::toTileSpace(lightpos.position), light.radius);

		for (auto entity : _toLight)
		{
			if (auto lit = registry->try_get<component::Lit>(entity))
			{
				lit->color.r = std::clamp((lit->color.r + light.color.r) / 2, 0, 255);
				lit->color.g = std::clamp((lit->color.g + light.color.g) / 2, 0, 255);
				lit->color.b = std::clamp((lit->color.b + light.color.b) / 2, 0, 255);
			}
			else
			{
				registry->emplace<component::Lit>(entity, light.color);
			}
		}
		_toLight.clear();
	}

	_lightBlockingPositions.clear();
}
