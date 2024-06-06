#include "pch.h"
#include "LiquidSystem.h"

#include "Components/Components.h"
#include "Components/Tags.h"
#include "Components/LiquidComponent.h"
#include "Components/InLiquidComponent.h"
#include "Components/PositionComponent.h"
#include "Components/RenderComponent.h"
#include "Components/HealthComponent.h"
#include "Components/StaminaComponent.h"
#include "Components/MaterialComponent.h"
#include "Components/FlyingComponent.h"

#include "Spatial/Conversions.h"
#include "Spatial/Helpers.h"
#include "Spatial/WorldGrid.h"
#include "Utility/SpriteIndexer.h"
#include "Utility/stdHashing.h"

void drft::system::LiquidSystem::init()
{
	_grid = &(_registry->ctx().get<spatial::WorldGrid&>());

	_dispatcher->sink<events::TurnEndEvent>().connect<&LiquidSystem::onTurnEndEvent>(this);

	_registry->on_construct<InLiquidComponent>().connect<&LiquidSystem::onUpdateInLiquid>(this);
	_registry->on_destroy<InLiquidComponent>().connect<&LiquidSystem::onRemoveInLiquid>(this);
}

void drft::system::LiquidSystem::fixedUpdate()
{
	auto inViewportLiquidView = _registry->view<LiquidComponent, PositionComponent, RenderComponent, component::tag::InViewport>();
	entt::dense_set<sf::Vector2i> liquidPositions;
	for (auto [entity, liquid, pos, render] : inViewportLiquidView.each())
	{
		liquidPositions.insert(pos.position);
		if (liquid.volume > 500)
		{
			auto entities = _grid->entitiesAt(pos.position,
				[this](entt::entity entity) -> bool
				{
					return LiquidSystem::isAffectedByLiquids({ *_registry, entity });
				});
			if (!entities.empty())
			{
				addInLiquidEffect(pos.position, render.color);
			}
			for (auto&& entity : entities)
			{
				_registry->emplace_or_replace<InLiquidComponent>(entity, liquid.volume);
			}
		}
	}

	auto outOfViewportLiquidView = _registry->view<LiquidComponent, PositionComponent>(entt::exclude<component::tag::InViewport>);
	for (auto [entity, liquid, pos] : outOfViewportLiquidView.each())
	{
		liquidPositions.insert(pos.position);
	}

	auto inLiquidView = _registry->view<InLiquidComponent, PositionComponent>();
	for (auto [entity, inLiquid, pos] : inLiquidView.each())
	{
		if (!liquidPositions.contains(pos.position)) continue;
		_registry->remove<InLiquidComponent>(entity);
	}
}

void drft::system::LiquidSystem::onFixedUpdateEnd()
{
	for (auto e : _inLiquidEffects)
	{
		_registry->destroy(e);
	}
	_inLiquidEffects.clear();
}

bool drft::system::LiquidSystem::isAffectedByLiquids(entt::const_handle entity)
{
	if (!entity.any_of<LiquidComponent, FlyingComponent>()
		&& entity.all_of<MaterialComponent>())
	{
		return true;
	}
	return false;
}

void drft::system::LiquidSystem::addInLiquidEffect(sf::Vector2i position, sf::Color color)
{
	sf::Color translucentColor = { color.r, color.g, color.b, 200 };
	auto effect = entt::handle{ *_registry, _registry->create() };
	RenderComponent renderComponent =
	{
		.texture = entt::hashed_string("simpleTileset"),
		.uvSize = {16, 16},
		.uvCoords = {8, 4},
		.layer = 4u,
		.color = translucentColor
	};
	effect.emplace<RenderComponent>(renderComponent);
	effect.emplace<PositionComponent>(position);
	effect.emplace<component::tag::InViewport>();
	_inLiquidEffects.push_back(effect.entity());
}

void drft::system::LiquidSystem::onTurnEndEvent(events::TurnEndEvent& ev) const
{
	auto health = _registry->try_get<HealthComponent>(ev.entity);
	auto stamina = _registry->try_get<StaminaComponent>(ev.entity);
	auto isInLiquid = _registry->all_of<InLiquidComponent>(ev.entity);

	if (health && stamina && isInLiquid 
		&& LiquidSystem::isAffectedByLiquids({ *_registry, ev.entity })
		&& stamina->current <= 0.f)
	{
		_registry->emplace_or_replace<component::action::TakeDamage>(ev.entity, static_cast<int>(health->max / 10));
	}
}

void drft::system::LiquidSystem::onUpdateInLiquid(entt::registry& registry, entt::entity entity)
{
	if (auto stamina = registry.try_get<StaminaComponent>(entity))
	{
		stamina->baseConsumption += 1.f;
	}
}

void drft::system::LiquidSystem::onRemoveInLiquid(entt::registry& registry, entt::entity entity)
{
	if (auto stamina = registry.try_get<StaminaComponent>(entity))
	{
		stamina->baseConsumption -= 1.f;
	}
}
