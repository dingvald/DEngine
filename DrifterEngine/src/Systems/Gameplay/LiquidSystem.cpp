#include "pch.h"
#include "LiquidSystem.h"

#include "Systems/Rendering/RenderLayers.h"

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
	_grid = &(_registry.ctx().get<spatial::WorldGrid&>());

	_dispatcher.sink<events::TurnEndEvent>().connect<&LiquidSystem::onTurnEndEvent>(this);

	_registry.on_construct<InLiquidComponent>().connect<&LiquidSystem::onUpdateInLiquid>(this);
	_registry.on_destroy<InLiquidComponent>().connect<&LiquidSystem::onRemoveInLiquid>(this);
}

void drft::system::LiquidSystem::onFixedUpdate()
{
	auto liquidAffectedView = _registry.view<MaterialComponent, PositionComponent>(entt::exclude<LiquidComponent, FlyingComponent>);
	for (auto [entity, material, pos] : liquidAffectedView.each())
	{
		auto liquidEntity = getLiquidAt(pos.tile);
		if (auto liquid = _registry.try_get<LiquidComponent>(liquidEntity))
		{
			_registry.emplace_or_replace<InLiquidComponent>(entity, liquid->volume);
		}
	}

	auto inLiquidView = _registry.view<InLiquidComponent, PositionComponent>();
	for (auto [entity, inLiquid, pos] : inLiquidView.each())
	{
		auto liquidEntity = getLiquidAt(pos.tile);
		if (liquidEntity != entt::null && inLiquid.volume > 200.0f)
		{
			if (auto render = _registry.try_get<RenderComponent>(liquidEntity))
			{
				addInLiquidEffect(pos.tile, render->color);
			}
		}
		else
		{
			_registry.remove<InLiquidComponent>(entity);
		}
	}
}

void drft::system::LiquidSystem::onFixedUpdateEnd()
{
	for (auto e : _inLiquidEffects)
	{
		_registry.destroy(e);
	}
	_inLiquidEffects.clear();
}

void drft::system::LiquidSystem::addInLiquidEffect(sf::Vector3i position, sf::Color color)
{
	sf::Color translucentColor = { color.r, color.g, color.b, 200 };
	auto effect = entt::handle{ _registry, _registry.create() };
	RenderComponent renderComponent =
	{
		.texture = entt::hashed_string("liquid"),
		.uvSize = DefaultTileTextureSize,
		.uvCoords = {2, 0},
		.layer = 5u,
		.color = translucentColor
	};
	effect.emplace<RenderComponent>(renderComponent);
	effect.emplace<PositionComponent>(spatial::asTileSpace(position));
	effect.emplace<component::tag::InViewport>();
	_inLiquidEffects.push_back(effect.entity());
}

entt::entity drft::system::LiquidSystem::getLiquidAt(TilePosition tilePosition) const
{
	auto liquids = _grid->entitiesAt(tilePosition, 
		[this](entt::entity entity) -> bool {
			return _registry.all_of<LiquidComponent>(entity);
		});
	return liquids.empty() ? entt::null : liquids.front();
}

void drft::system::LiquidSystem::onTurnEndEvent(events::TurnEndEvent& ev) const
{
	auto health = _registry.try_get<HealthComponent>(ev.entity);
	auto stamina = _registry.try_get<StaminaComponent>(ev.entity);
	auto isInLiquid = _registry.all_of<InLiquidComponent>(ev.entity);

	if (health && stamina && isInLiquid && stamina->current <= 0.f)
	{
		_registry.emplace_or_replace<component::action::TakeDamage>(ev.entity, static_cast<int>(health->max / 10));
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
