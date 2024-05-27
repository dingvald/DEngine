#include "pch.h"
#include "LightingSystem.h"

#include "Components/Components.h"
#include "Components/GlobalLightSourceComponent.h"
#include "Components/LocalLightSourceComponent.h"
#include "Components/TemporaryLightSourceComponent.h"
#include "Components/PositionComponent.h"
#include "Components/VisualEffectComponent.h"
#include "Components/LightBlockingComponent.h"
#include "Components/LitComponent.h"
#include "Components/Tags.h"

#include "Spatial/Helpers.h"
#include "Spatial/Conversions.h"
#include "Utility/Visibility.h"
#include "Spatial/WorldGrid.h"
#include "Spatial/Grid.h"

void drft::system::LightingSystem::init()
{
	auto& grid = _registry->ctx().get<spatial::WorldGrid&>();

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
	auto globalLightView = _registry->view<GlobalLightSourceComponent>();
	auto positions = _registry->view<const PositionComponent, component::tag::InViewport>(entt::exclude<VisualEffectComponent>);
	// Apply global lighting
	for (auto [_, globalLight] : globalLightView.each())
	{
		for (auto entity : positions)
		{
			if (auto lit = _registry->try_get<LitComponent>(entity))
			{
				lit->color = globalLight.color;
			}
			else
			{
				_registry->emplace<LitComponent>(entity, globalLight.color);
			}
		}
	}

	// Get all light blocking entities to be checked by the FOV algo
	_lightBlockingPositions.reserve(positions.size_hint() / 4); // Arbitrarily reserve a fourth of the positions.
	for (auto [entity, pos] : positions.each())
	{
		if (_registry->any_of<LightBlockingComponent>(entity))
		{
			_lightBlockingPositions.emplace(pos.position);
		}
	}

	// Apply light from local light sources
	auto lighting = _registry->view<const LocalLightSourceComponent, const PositionComponent, component::tag::InViewport>();
	for (auto [_, light, lightpos] : lighting.each())
	{
		_fov->compute(lightpos.position, static_cast<int>(light.radius));
		for (auto entity : _toLight)
		{
			auto& pos = _registry->get<PositionComponent>(entity);
			auto tileDistance = spatial::distance(pos.position, lightpos.position);
			float denom = (tileDistance / light.radius) + (1.f*light.dropOff);
			float i = std::clamp( 1 / (denom*denom), 0.0f, 1.0f);
			sf::Color lightColor = 
			{
				static_cast<sf::Uint8>(light.color.r * i),
				static_cast<sf::Uint8>(light.color.g * i),
				static_cast<sf::Uint8>(light.color.b * i)
			};
			if (auto lit = _registry->try_get<LitComponent>(entity))
			{
				lit->color = blendColor(lit->color, lightColor);
			}
			else
			{
				_registry->emplace<LitComponent>(entity, lightColor);
			}
		}
		_toLight.clear();
	}

	// Apply light from temporary light sources
	auto tempLighting = _registry->view<const TemporaryLightSourceComponent, const PositionComponent, component::tag::InViewport>();
	for (auto [_, light, lightpos] : tempLighting.each())
	{
		_fov->compute(lightpos.position, static_cast<int>(light.radius));
		for (auto entity : _toLight)
		{
			auto& pos = _registry->get<PositionComponent>(entity);
			auto tileDistance = spatial::distance(pos.position, lightpos.position);
			float denom = (tileDistance / light.radius) + (1.f * light.dropOff);
			float i = std::clamp(1 / (denom * denom), 0.0f, 1.0f);
			sf::Color lightColor =
			{
				static_cast<sf::Uint8>(light.color.r * i),
				static_cast<sf::Uint8>(light.color.g * i),
				static_cast<sf::Uint8>(light.color.b * i)
			};
			if (auto lit = _registry->try_get<LitComponent>(entity))
			{
				lit->color = LightingSystem::blendColor(lit->color, lightColor);
			}
			else
			{
				_registry->emplace<LitComponent>(entity, lightColor);
			}
		}
		_toLight.clear();
	}

	_lightBlockingPositions.clear();
}

void drft::system::LightingSystem::onFixedUpdateEnd()
{
	_registry->clear<LitComponent>();
}

sf::Color drft::system::LightingSystem::blendColor(const sf::Color& color1, const sf::Color& color2)
{
	sf::Color result;
	result.r = static_cast<sf::Uint8>(std::clamp(std::max(static_cast<int>(color1.r), (color1.r + color2.r) / 2), 0, 255));
	result.g = static_cast<sf::Uint8>(std::clamp(std::max(static_cast<int>(color1.g), (color1.g + color2.g) / 2), 0, 255));
	result.b = static_cast<sf::Uint8>(std::clamp(std::max(static_cast<int>(color1.b), (color1.b + color2.b) / 2), 0, 255));
	return result;
}
