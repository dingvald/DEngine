#include "pch.h"
#include "ProjectileSystem.h"
#include "Components/Components.h"
#include "Components/Tags.h"
#include "Systems/Helpers/SpawnEffect.h"

void drft::system::ProjectileSystem::init()
{
	_registry->on_construct<component::Projectile>().connect<&ProjectileSystem::onProjectileAdded>(this);
	_registry->on_destroy<component::Projectile>().connect<&ProjectileSystem::onProjectileRemoved>(this);
}

void drft::system::ProjectileSystem::update(float dt)
{
	auto view = _registry->view<component::Position, component::Projectile, component::tag::CurrentActor>();
	for (auto [entity, pos, proj] : view.each())
	{
		if (proj.progress >= proj.line.size())
		{
			_registry->remove<component::Projectile>(entity);
			continue;
		}

		auto delta = proj.line.at(proj.progress++) - pos.position;
		_registry->emplace_or_replace<component::action::Move>(entity, delta);

		if (auto render = _registry->try_get<component::Render>(entity))
		{
			spawnEffect(*_registry,
				{
					.color = sf::Color::White,
					.sprites = {render->sprite},
					.layer = RenderLayer::EffectsFront,
					.position = pos.position,
					.ttl = 30,
					.fades = true
				});
		}
	}
}

void drft::system::ProjectileSystem::onProjectileAdded(entt::registry& registry, entt::entity entity)
{
	auto& projectile = registry.get<component::Projectile>(entity);
	registry.emplace<component::Actor>(entity, 100, projectile.speed);

	if (registry.any_of<component::Attacker>(entity)) return;
	if (auto physical = registry.try_get<component::Physical>(entity))
	{
		registry.emplace<component::Attacker>(entity, static_cast<int>(physical->weight));
		_attackerAdded.insert(entity);
	}
}

void drft::system::ProjectileSystem::onProjectileRemoved(entt::registry& registry, entt::entity entity)
{
	registry.remove<component::Actor>(entity);
	if (_attackerAdded.contains(entity))
	{
		registry.remove<component::Attacker>(entity);
		_attackerAdded.erase(entity);
	}
}
