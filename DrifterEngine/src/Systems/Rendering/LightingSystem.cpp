#include "pch.h"
#include "LightingSystem.h"
#include "Components/Components.h"
#include "Components/Tags.h"
#include "Spatial/Helpers.h"
#include "Spatial/Conversions.h"

void drft::system::LightingSystem::init()
{
}

void drft::system::LightingSystem::fixedUpdate()
{
	auto lighting = registry->view<const component::LightSource, const component::Position, component::tag::InViewport>();
	auto position = registry->view<const component::Position, component::tag::InViewport>();
	for (auto [_, light, lightpos] : lighting.each())
	{
		for (auto [entity, pos] : position.each())
		{
			if (spatial::distance(spatial::toTileSpace(lightpos.position),
				spatial::toTileSpace(pos.position)) < light.radius)
			{
				registry->emplace_or_replace<component::Lit>(entity, light.color);
			}
		}
	}
}
