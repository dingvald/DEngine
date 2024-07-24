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

using namespace entt::literals;

static constexpr float PI = 3.141592f;

// TODO: Refactor sprintig animations into the visual effect system

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
		if (!_sprintEffects.contains(entity))
		{
			addSprintEffect(_registry, entity);
		}
		_registry.patch<PositionComponent>(_sprintEffects[entity],
			[pos](PositionComponent& position)
			{
				position.position = pos.position;
			});
	}

	animateSprintEffects();
}

void drft::system::SprintingSystem::shutdown()
{
	for (auto&& [sprinter, effect] : _sprintEffects)
	{
		_registry.destroy(effect);
	}
}

void drft::system::SprintingSystem::onSprintingAdded(entt::registry& registry, entt::entity entity)
{
	applySprintBuff(registry, entity);
	addSprintEffect(registry, entity);
	registry.emplace_or_replace<SpriteChangeRequestComponent>(entity, "sprinting"_hs);
}

void drft::system::SprintingSystem::onSprintingRemoved(entt::registry& registry, entt::entity entity)
{
	removeSprintBuff(registry, entity);
	removeSprintEffect(registry, entity);
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

void drft::system::SprintingSystem::addSprintEffect(entt::registry& registry, entt::entity entity)
{
	if (_sprintEffects.contains(entity)) return;
	auto effect = entt::handle{ registry, registry.create() };
	RenderComponent renderComponent =
	{
		.texture = entt::hashed_string("simple_tileset"),
		.uvSize = {16, 16},
		.uvCoords = {1, 5},
		.layer = 4u,
		.color = { 50,150,50 }
	};
	effect.emplace<RenderComponent>(std::move(renderComponent));
	effect.emplace<PositionComponent>(sf::Vector2i(0,0));
	_sprintEffects.emplace(entity, effect.entity());
}

void drft::system::SprintingSystem::removeSprintEffect(entt::registry& registry, entt::entity entity)
{
	registry.destroy(_sprintEffects.at(entity));
	_sprintEffects.erase(entity);
}

void drft::system::SprintingSystem::animateSprintEffects() const
{
	static int frames = 0;
	if (frames >= 360) frames = 0;
	for (auto&& [sprinter, effect] : _sprintEffects)
	{
		auto& render = _registry.get<RenderComponent>(effect);
		float alpha = 255 * ((std::sinf(frames * (PI / 180.f)) + 1.f) / 2.f);
		render.color.a = static_cast<sf::Uint8>(alpha);
	}
	frames+=5;
}


