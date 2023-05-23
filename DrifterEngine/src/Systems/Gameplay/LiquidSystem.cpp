#include "pch.h"
#include "LiquidSystem.h"
#include "Components/Components.h"
#include "Components/Tags.h"
#include "Spatial/Conversions.h"
#include "Spatial/Helpers.h"
#include "Spatial/WorldGrid.h"
#include "Utility/SpriteIndexer.h"

void drft::system::LiquidSystem::init()
{
	_grid = &(registry->ctx().get<spatial::WorldGrid&>());
}

void drft::system::LiquidSystem::fixedUpdate()
{
	auto liquidView = registry->view<component::Liquid, component::Position, component::Render, component::tag::InViewport>();
	for (auto [entity, liquid, pos, render] : liquidView.each())
	{
		if (liquid.volume > 500)
		{
			auto entities = _grid->entitiesAt(spatial::toTileSpace(pos.position),
				[this](entt::entity entity) -> bool
				{
					if (!registry->any_of<component::Liquid, component::Flying>(entity)
						&& registry->any_of<component::Physical>(entity))
					{
						return true;
					}
					return false;
				});

			if (!entities.empty())
			{
				addInLiquidEffect(pos.position, render.color);
			}
		}
	}
}

void drft::system::LiquidSystem::onFixedUpdateEnd()
{
	for (auto e : _inLiquidEffects)
	{
		registry->destroy(e);
	}
	_inLiquidEffects.clear();
}

void drft::system::LiquidSystem::addInLiquidEffect(sf::Vector2f position, sf::Color color)
{
	sf::Color translucentColor = { color.r, color.g, color.b, 200 };
	auto effect = entt::handle{ *registry, registry->create() };
	effect.emplace<component::Render>(static_cast<unsigned int>(util::Sprite::InLiquidEffect), 4u, translucentColor);
	effect.emplace<component::Position>(position);
	effect.emplace<component::tag::InViewport>();
	_inLiquidEffects.push_back(effect.entity());
}
