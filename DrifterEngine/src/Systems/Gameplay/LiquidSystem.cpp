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
	dispatcher.sink<events::TurnEndEvent>().connect<&LiquidSystem::onTurnEndEvent>(this);

	registry->on_construct<component::InLiquid>().connect<&LiquidSystem::onUpdateInLiquid>(this);
	registry->on_destroy<component::InLiquid>().connect<&LiquidSystem::onRemoveInLiquid>(this);
}

void drft::system::LiquidSystem::fixedUpdate()
{
	auto liquidView = registry->view<component::Liquid, component::Position, component::Render, component::tag::InViewport>();
	for (auto [entity, liquid, pos, render] : liquidView.each())
	{
		if (liquid.volume > 500)
		{
			auto entities = _grid->entitiesAt(pos.position,
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

	auto inLiquidView = registry->view<component::InLiquid, component::Position>();
	for (auto [entity, inLiquid, pos] : inLiquidView.each())
	{
		auto liquids = _grid->entitiesAt(pos.position,
			[this](auto entity) -> bool
			{
				return registry->any_of<component::Liquid>(entity);
			});
		if (liquids.empty())
		{
			registry->remove<component::InLiquid>(entity);
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

void drft::system::LiquidSystem::addInLiquidEffect(sf::Vector2i position, sf::Color color)
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

	if (!liquids.empty() && isAffectedByLiquids(ev.entity))
	{
		auto& liquid = registry->get<component::Liquid>(liquids.front());
		auto& prototype = registry->get<component::Prototype>(liquids.front());
		registry->emplace_or_replace<component::InLiquid>(ev.entity, prototype.name, liquid.volume);
	}
}

void drft::system::LiquidSystem::onTurnEndEvent(events::TurnEndEvent& ev) const
{
	auto health = registry->try_get<component::Health>(ev.entity);
	auto stamina = registry->try_get<component::Stamina>(ev.entity);
	auto isInLiquid = registry->all_of<component::InLiquid>(ev.entity);

	if (health && stamina && isInLiquid 
		&& isAffectedByLiquids(ev.entity) 
		&& stamina->current <= 0.f)
	{
		registry->emplace_or_replace<component::action::TakeDamage>(ev.entity, static_cast<int>(health->max / 10));
	}
}

void drft::system::LiquidSystem::onUpdateInLiquid(entt::registry& registry, entt::entity entity)
{
	if (auto stamina = registry.try_get<component::Stamina>(entity))
	{
		stamina->baseConsumption += 1.f;
	}
}

void drft::system::LiquidSystem::onRemoveInLiquid(entt::registry& registry, entt::entity entity)
{
	if (auto stamina = registry.try_get<component::Stamina>(entity))
	{
		stamina->baseConsumption -= 1.f;
	}
}
