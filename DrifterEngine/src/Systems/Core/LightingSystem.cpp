#include "pch.h"
#include "LightingSystem.h"
#include "Components/Components.h"
#include "Spatial/Helpers.h"
#include "Spatial/Conversions.h"

void drft::system::LightingSystem::init()
{
}

void drft::system::LightingSystem::fixedUpdate()
{
	sf::FloatRect viewport;
	sf::FloatRect bufferdViewport;
	const auto camera = registry->view<const component::Camera, const component::Position>();

	// assumes only one camera, otherwise will just use the last one's position
	for (auto const& [entity, cam, pos] : camera.each())
	{
		viewport = cam.viewport;
		bufferdViewport = {
			viewport.left - 32,
			viewport.top - 32,
			viewport.width + 64,
			viewport.height + 64
		};
	}

	auto lighting = registry->view<component::LightSource, component::Position>();
	auto position = registry->view<component::Position>();
	for (auto [_, light, lightpos] : lighting.each())
	{
		if (!bufferdViewport.contains(lightpos.position)) continue;
		for (auto [entity, pos] : position.each())
		{
			if (!bufferdViewport.contains(pos.position)) continue;
			if (spatial::distance(spatial::toTileSpace(lightpos.position),
				spatial::toTileSpace(pos.position)) < light.radius)
			{
				registry->emplace_or_replace<component::Lit>(entity, light.color);
			}
		}
	}
}
