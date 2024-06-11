#include "pch.h"
#include "MeleeAttackActionSystem.h"
#include "Systems/Helpers/SpendActionPoints.h"
#include "Spatial/WorldGrid.h"
#include "Spatial/Conversions.h"

#include "Components/Components.h"
#include "Components/Actions/MeleeAttackAction.h"
#include "Components/AttackerComponent.h"
#include "Components/PositionComponent.h"
#include "Components/ProjectileComponent.h"

#include "Components/Tags.h"
#include "Utility/EntityHelpers.h"
#include "Utility/SpriteOptions.h"
#include "Systems/Helpers/SpawnEffect.h"

using namespace entt::literals;

void drft::system::MeleeAttackActionSystem::init()
{
	_registry->on_construct<PerformMeleeAttackAction>().connect<&MeleeAttackActionSystem::onPerformMeleeAttackAction>(this);
	_registry->on_construct<TryMeleeAttackAction>().connect<&MeleeAttackActionSystem::onTryMeleeAttackAction>(this);
	_registry->on_construct<DoMeleeAttackAction>().connect<&MeleeAttackActionSystem::onDoMeleeAttackAction>(this);
}

void drft::system::MeleeAttackActionSystem::onUpdateEnd()
{
	_registry->clear<PerformMeleeAttackAction>();
	_registry->clear<TryMeleeAttackAction>();
	_registry->clear<DoMeleeAttackAction>();
}

void drft::system::MeleeAttackActionSystem::onPerformMeleeAttackAction(entt::registry& registry, entt::entity entity) const
{
	const auto& performMeleeAttack = registry.get<PerformMeleeAttackAction>(entity);
	const auto& tryMeleeAttack = registry.emplace_or_replace<TryMeleeAttackAction>(entity, performMeleeAttack.direction, performMeleeAttack.targets);
	if (!tryMeleeAttack.cancel)
	{
		registry.emplace_or_replace<DoMeleeAttackAction>(entity, tryMeleeAttack.direction, tryMeleeAttack.targets, tryMeleeAttack.damageTypes);
	}
}

void drft::system::MeleeAttackActionSystem::onTryMeleeAttackAction(entt::registry& registry, entt::entity entity) const
{
	auto& tryMeleeAttack = registry.get<TryMeleeAttackAction>(entity);
	if (auto attackerComponent = registry.try_get<AttackerComponent>(entity))
	{
		
		tryMeleeAttack.damageTypes["crushing"] += attackerComponent->baseDamage;
	}
}

void drft::system::MeleeAttackActionSystem::onDoMeleeAttackAction(entt::registry& registry, entt::entity entity) const
{
	auto& doAttackAction = registry.get<DoMeleeAttackAction>(entity);
	for (auto target : doAttackAction.targets)
	{
		_registry->emplace_or_replace<component::action::IncomingDamage>(target, doAttackAction.damageTypes, entity);
	}

	if (auto positionComponent = registry.try_get<PositionComponent>(entity))
	{
		sf::Color effectColor = sf::Color::White;

		std::vector<SpriteOptions> frames = // crushing
		{
			SpriteOptions{.uvCoords = sf::Vector2i{8, 0}, .texture = "simple_tileset"_hs, .uvSize = sf::Vector2i{16, 16}, .layer = static_cast<unsigned int>(RenderLayer::EffectsBack), .color = effectColor},
			SpriteOptions{.uvCoords = sf::Vector2i{9, 0}, .texture = "simple_tileset"_hs, .uvSize = sf::Vector2i{16, 16}, .layer = static_cast<unsigned int>(RenderLayer::EffectsBack), .color = effectColor},
		};
		if (doAttackAction.damageTypes.contains("slashing")
			|| doAttackAction.damageTypes.contains("piercing"))
		{
			frames = // slashing
			{
				SpriteOptions{.uvCoords = sf::Vector2i{6, 1}, .texture = "simple_tileset"_hs, .uvSize = sf::Vector2i{16, 16}, .layer = static_cast<unsigned int>(RenderLayer::EffectsBack), .color = effectColor},
				SpriteOptions{.uvCoords = sf::Vector2i{7, 1}, .texture = "simple_tileset"_hs, .uvSize = sf::Vector2i{16, 16}, .layer = static_cast<unsigned int>(RenderLayer::EffectsBack), .color = effectColor},
			};
		}
		const sf::Vector2i targetPosition = positionComponent->position + doAttackAction.direction;
		spawnEffect(*_registry, {
			.frames = frames,
			.position = targetPosition,
			.animationSpeed = 20.0f
			});
	}
	
	spendActionPoints(BASE_ACTION_COST, ActionType::Act, { *_registry, entity });
}
