#include "pch.h"
#include "ProjectileSystem.h"

#include "Components/Actions/MeleeAttackAction.h"
#include "Components/Actions/MoveAction.h"
#include "Components/ProjectileComponent.h"
#include "Components/PositionComponent.h"
#include "Components/RenderComponent.h"
#include "Components/AttackerComponent.h"
#include "Components/ActorComponent.h"
#include "Components/MaterialComponent.h"
#include "Components/Tags.h"

#include "Systems/Helpers/SpawnEffect.h"
#include "Utility/SpriteOptions.h"

void drft::system::ProjectileSystem::init()
{
	_registry.on_construct<MeleeAttackAction>().connect<&ProjectileSystem::onMeleeAttackActionAdded>(this);
	_registry.on_construct<ProjectileComponent>().connect<&ProjectileSystem::onProjectileAdded>(this);
	_registry.on_destroy<ProjectileComponent>().connect<&ProjectileSystem::onProjectileRemoved>(this);
}

void drft::system::ProjectileSystem::onUpdate(float dt)
{
	auto view = _registry.view<PositionComponent, ProjectileComponent, component::tag::CurrentActor>();
	for (auto [entity, pos, proj] : view.each())
	{
		if (proj.progress >= proj.line.size())
		{
			_registry.remove<ProjectileComponent>(entity);
			continue;
		}

		auto delta = proj.line.at(proj.progress++) - pos.position;
		_registry.emplace_or_replace<MoveAction>(entity, delta);

		if (auto render = _registry.try_get<RenderComponent>(entity))
		{
			SpriteOptions effectSprite;
			createSpriteOptionsFromRenderComponent(effectSprite, *render);
			effectSprite.layer = static_cast<unsigned int>(RenderLayer::EffectsFront);
			effectSprite.color = sf::Color::White;
			effectSprite.color.value().a = 90;

			spawnEffect(_registry,
				{
					.frames = { effectSprite },
					.position = pos.position,
					.ttl = 30,
					.fadeRate = 3
				});
		}
	}
}

void drft::system::ProjectileSystem::onProjectileAdded(entt::registry& registry, entt::entity entity)
{
	auto& projectile = registry.get<ProjectileComponent>(entity);
	registry.emplace<ActorComponent>(entity, 100, projectile.speed);

	if (registry.any_of<AttackerComponent>(entity)) return;

	if (auto material = registry.try_get<MaterialComponent>(entity))
	{
		registry.emplace<AttackerComponent>(entity, static_cast<int>(material->weight));
		_attackerAdded.insert(entity);
	}
}

void drft::system::ProjectileSystem::onProjectileRemoved(entt::registry& registry, entt::entity entity)
{
	registry.remove<ActorComponent>(entity);
	if (_attackerAdded.contains(entity))
	{
		registry.remove<AttackerComponent>(entity);
		_attackerAdded.erase(entity);
	}
}

void drft::system::ProjectileSystem::onMeleeAttackActionAdded(entt::registry& registry, entt::entity entity)
{
	registry.remove<ProjectileComponent>(entity);
}
