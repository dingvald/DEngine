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
	auto& dispatcher = registry->ctx().get<entt::dispatcher&>();
	dispatcher.sink<events::EnterTileEvent>().connect<&LiquidSystem::onEnterTileEvent>(this);
	dispatcher.sink<events::LeaveTileEvent>().connect<&LiquidSystem::onLeaveTileEvent>(this);
	dispatcher.sink<events::TurnEndEvent>().connect<&LiquidSystem::onTurnEndEvent>(this);
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
					return isAffectedByLiquids(entity);
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

bool drft::system::LiquidSystem::isAffectedByLiquids(entt::entity entity) const
{
	if (!registry->any_of<component::Liquid, component::Flying>(entity)
		&& registry->all_of<component::Physical>(entity))
	{
		return true;
	}
	return false;
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

void drft::system::LiquidSystem::onEnterTileEvent(events::EnterTileEvent& ev) const
{
	auto liquids = _grid->entitiesAt(ev.tilePosition,
		[this](auto entity) -> bool
		{
			return registry->any_of<component::Liquid>(entity);
		});
	if (auto stamina = registry->try_get<component::Stamina>(ev.entity))
	{
		if (!liquids.empty() && isAffectedByLiquids(ev.entity))
		{
			stamina->baseConsumption += 2;
		}
	}
}

void drft::system::LiquidSystem::onLeaveTileEvent(events::LeaveTileEvent& ev) const
{
	auto liquids = _grid->entitiesAt(ev.tilePosition,
		[this](auto entity) -> bool
		{
			return registry->any_of<component::Liquid>(entity);
		});
	if (auto stamina = registry->try_get<component::Stamina>(ev.entity))
	{
		if (!liquids.empty() && isAffectedByLiquids(ev.entity))
		{
			stamina->baseConsumption -= 2;
		}
	}
}

void drft::system::LiquidSystem::onTurnEndEvent(events::TurnEndEvent& ev) const
{
	if (auto pos = registry->try_get<component::Position>(ev.entity))
	{
		auto health = registry->try_get<component::Health>(ev.entity);
		auto stamina = registry->try_get<component::Stamina>(ev.entity);
		auto liquids = _grid->entitiesAt(spatial::toTileSpace(pos->position),
			[this](auto entity) -> bool
			{
				return registry->any_of<component::Liquid>(entity);
			});

		if (health && stamina && !liquids.empty() 
			&& isAffectedByLiquids(ev.entity) 
			&& stamina->current <= 0)
		{
			registry->emplace_or_replace<component::action::TakeDamage>(ev.entity, health->max / 10);
		}
	}
}
