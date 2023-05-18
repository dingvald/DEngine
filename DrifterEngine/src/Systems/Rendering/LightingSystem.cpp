#include "pch.h"
#include "LightingSystem.h"
#include "Components/Components.h"
#include "Components/Tags.h"
#include "Spatial/Helpers.h"
#include "Spatial/Conversions.h"
#include "Utility/Visibility.h"
#include "Spatial/WorldGrid.h"
#include "Spatial/Grid.h"

void drft::system::LightingSystem::init()
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
		return static_cast<int>(spatial::distance({0,0}, position));
	};

	_fov = std::make_unique<Visibility>(blocksLight, setVisible, getDistance);
}

void drft::system::LightingSystem::fixedUpdate()
{
	auto globalLightView = registry->view<component::GlobalLightSource>();
	auto positions = registry->view<const component::Position, component::tag::InViewport>();

	for (auto [entity, globalLight] : globalLightView.each())
	{
		for (auto entity : positions)
		{
			if (auto lit = registry->try_get<component::Lit>(entity))
			{
				lit->color = globalLight.color;
			}
			else
			{
				registry->emplace<component::Lit>(entity, globalLight.color);
			}
		}
	}
	
	_lightBlockingPositions.reserve(positions.size_hint());
	for (auto [entity, pos] : positions.each())
	{
		if (registry->any_of<component::LightBlocking>(entity))
		{
			_lightBlockingPositions.emplace(spatial::toTileSpace(pos.position));
		}
	}

	auto lighting = registry->view<const component::LightSource, const component::Position, component::tag::InViewport>();
	for (auto [_, light, lightpos] : lighting.each())
	{
		_fov->compute(spatial::toTileSpace(lightpos.position), light.radius);
		for (auto entity : _toLight)
		{
			auto pos = registry->get<component::Position>(entity);
			auto tileDistance = spatial::distance(spatial::toTileSpace(pos.position), spatial::toTileSpace(lightpos.position));
			float denom = (tileDistance / light.radius) + (1.f*light.dropOff);
			float i = std::clamp( 1 / (denom*denom), 0.0f, 1.0f);
			sf::Color lightColor = 
			{
				static_cast<sf::Uint8>(light.color.r * i),
				static_cast<sf::Uint8>(light.color.g * i),
				static_cast<sf::Uint8>(light.color.b * i)
			};
			if (auto lit = registry->try_get<component::Lit>(entity))
			{
				lit->color.r = std::clamp(std::max(static_cast<int>(lit->color.r), (lit->color.r + lightColor.r) / 2), 0, 255);
				lit->color.g = std::clamp(std::max(static_cast<int>(lit->color.g), (lit->color.g + lightColor.g) / 2), 0, 255);
				lit->color.b = std::clamp(std::max(static_cast<int>(lit->color.b), (lit->color.b + lightColor.b) / 2), 0, 255);
			}
			else
			{
				registry->emplace<component::Lit>(entity, lightColor);
			}
		}
		_toLight.clear();
	}
	_lightBlockingPositions.clear();
}
