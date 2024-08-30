#include "pch.h"
#include "SprintingSystem.h"

#include "Components/Components.h"
#include "Components/SprintingComponent.h"
#include "Components/PositionComponent.h"
#include "Components/StaminaComponent.h"
#include "Components/ActorComponent.h"
#include "Components/RenderComponent.h"
#include "Components/SpriteChangeRequestComponent.h"

#include "Utility/SpriteIndexer.h"
#include "Systems/Helpers/SpawnEffect.h"
#include "Utility/SpriteOptions.h"

using namespace entt::literals;

static constexpr float PI = 3.141592f;

void drft::system::SprintingSystem::init()
{
	_registry.on_construct<SprintingComponent>().connect<&SprintingSystem::onSprintingAdded>(this);
	_registry.on_destroy<SprintingComponent>().connect<&SprintingSystem::onSprintingRemoved>(this);
}

void drft::system::SprintingSystem::onFixedUpdate()
{
	auto sprintView = _registry.view<SprintingComponent, StaminaComponent, const PositionComponent>();
	for (auto [entity, sprinting, stamina, pos] : sprintView.each())
	{
		if (stamina.current <= 0.f)
		{
			_registry.remove<SprintingComponent>(entity);
			continue;
		}

		if (auto render = _registry.try_get<RenderComponent>(entity))
		{
			spawnSprintEffect(*render, pos);
		}
	}
}

void drft::system::SprintingSystem::onSprintingAdded(entt::registry& registry, entt::entity entity)
{
	applySprintBuff(registry, entity);
	registry.emplace_or_replace<SpriteChangeRequestComponent>(entity, "sprinting"_hs);
}

void drft::system::SprintingSystem::onSprintingRemoved(entt::registry& registry, entt::entity entity)
{
	removeSprintBuff(registry, entity);
	registry.emplace_or_replace<SpriteChangeRequestComponent>(entity, "default"_hs);
}

void drft::system::SprintingSystem::applySprintBuff(entt::registry& registry, entt::entity entity)
{
	if (auto actor = registry.try_get<ActorComponent>(entity))
	{
		actor->moveSpeed += 2.f;
	}
	if (auto stamina = registry.try_get<StaminaComponent>(entity))
	{
		stamina->baseConsumption += 1.f;
	}
}

void drft::system::SprintingSystem::removeSprintBuff(entt::registry& registry, entt::entity entity)
{
	if (auto actor = registry.try_get<ActorComponent>(entity))
	{
		actor->moveSpeed -= 2.f;
	}
	if (auto stamina = registry.try_get<StaminaComponent>(entity))
	{
		stamina->baseConsumption -= 1.f;
	}
}

void drft::system::SprintingSystem::spawnSprintEffect(const RenderComponent& renderComponent, const PositionComponent& positionComponent) const
{
	SpriteOptions effectSprite;
	createSpriteOptionsFromRenderComponent(effectSprite, renderComponent);
	effectSprite.layer = 1;
	effectSprite.color.value().a = 50;

	EffectStruct sprintEffect = { .frames = { effectSprite }, .position = positionComponent.position, .ttl = 25, .fades = true };

	spawnEffect(_registry, std::move(sprintEffect));
}


