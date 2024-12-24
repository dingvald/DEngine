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
#include "Spatial/Grid2d.h"

void drft::system::LightingSystem::init()
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
		return static_cast<int>(spatial::distance3d({0,0,0}, position));
	};

	_fov = std::make_unique<Visibility>(blocksLight, setVisible, getDistance);
}

void drft::system::LightingSystem::render(sf::RenderTarget& target)
{
	_registry.clear<LitComponent>();

	auto globalLightView = _registry.view<GlobalLightSourceComponent>();
	auto positions = _registry.view<const PositionComponent, component::tag::InViewport>(entt::exclude<VisualEffectComponent>);
	// Apply global lighting
	for (auto [_, globalLight] : globalLightView.each())
	{
		for (auto entity : positions)
		{
			_registry.emplace_or_replace<LitComponent>(entity, globalLight.color);
		}
	}

	// Get all light blocking entities to be checked by the FOV algo
	auto lightBlockingPositions = _registry.view<const PositionComponent, LightBlockingComponent, component::tag::InViewport>();
	_lightBlockingPositions.reserve(lightBlockingPositions.size_hint());
	for (auto&& [entity, pos, lightBlocking] : lightBlockingPositions.each())
	{
		_lightBlockingPositions.emplace(pos.tile);
	}

	// Apply light from local light sources
	auto lighting = _registry.view<const LocalLightSourceComponent, const PositionComponent, component::tag::InViewport>();
	for (auto [_, light, lightpos] : lighting.each())
	{
		_fov->compute(lightpos.tile, static_cast<int>(light.radius));
		for (auto entity : _toLight)
		{
			auto& pos = _registry.get<PositionComponent>(entity);
			auto tileDistance = spatial::distance3d(pos.tile, lightpos.tile);
			float denom = (tileDistance / light.radius) + (1.f*light.dropOff);
			float i = std::clamp( 1 / (denom*denom), 0.0f, 1.0f);
			sf::Color lightColor = 
			{
				static_cast<sf::Uint8>(light.color.r * i),
				static_cast<sf::Uint8>(light.color.g * i),
				static_cast<sf::Uint8>(light.color.b * i)
			};
			if (auto lit = _registry.try_get<LitComponent>(entity))
			{
				lit->color = LightingSystem::blendColor(lit->color, lightColor);
			}
			else
			{
				_registry.emplace<LitComponent>(entity, lightColor);
			}
		}
		_toLight.clear();
	}

	// Apply light from temporary light sources
	auto tempLighting = _registry.view<const TemporaryLightSourceComponent, const PositionComponent, component::tag::InViewport>();
	for (auto [_, light, lightpos] : tempLighting.each())
	{
		_fov->compute(lightpos.tile, static_cast<int>(light.radius));
		for (auto entity : _toLight)
		{
			auto& pos = _registry.get<PositionComponent>(entity);
			auto tileDistance = spatial::distance3d(pos.tile, lightpos.tile);
			float denom = (tileDistance / light.radius) + (1.f * light.dropOff);
			float i = std::clamp(1 / (denom * denom), 0.0f, 1.0f);
			sf::Color lightColor =
			{
				static_cast<sf::Uint8>(light.color.r * i),
				static_cast<sf::Uint8>(light.color.g * i),
				static_cast<sf::Uint8>(light.color.b * i)
			};
			if (auto lit = _registry.try_get<LitComponent>(entity))
			{
				lit->color = LightingSystem::blendColor(lit->color, lightColor);
			}
			else
			{
				_registry.emplace<LitComponent>(entity, lightColor);
			}
		}
		_toLight.clear();
	}

	_lightBlockingPositions.clear();
}

sf::Color drft::system::LightingSystem::blendColor(const sf::Color& color1, const sf::Color& color2)
{
	sf::Color result;
	result.r = static_cast<sf::Uint8>(std::clamp(std::max(static_cast<int>(color1.r), (color1.r + color2.r) / 2), 0, 255));
	result.g = static_cast<sf::Uint8>(std::clamp(std::max(static_cast<int>(color1.g), (color1.g + color2.g) / 2), 0, 255));
	result.b = static_cast<sf::Uint8>(std::clamp(std::max(static_cast<int>(color1.b), (color1.b + color2.b) / 2), 0, 255));
	return result;
}

sf::Color drft::system::LightingSystem::blendLight(const sf::Color& color1, const sf::Color& color2)
{
	sf::Color result;
	result.r = static_cast<sf::Uint8>(std::clamp(color1.r * (static_cast<float>(color2.r) / 255.f), 0.f, 255.f));
	result.g = static_cast<sf::Uint8>(std::clamp(color1.g * (static_cast<float>(color2.g) / 255.f), 0.f, 255.f));
	result.b = static_cast<sf::Uint8>(std::clamp(color1.b * (static_cast<float>(color2.b) / 255.f), 0.f, 255.f));
	return result;
}
