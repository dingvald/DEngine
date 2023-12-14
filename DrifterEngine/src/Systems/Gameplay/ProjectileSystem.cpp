#include "pch.h"
#include "ProjectileSystem.h"
#include "Components/Components.h"
#include "Components/Tags.h"

void drft::system::ProjectileSystem::init()
{
	registry->on_construct<component::Projectile>().connect<&ProjectileSystem::onProjectileAdded>(this);
	registry->on_destroy<component::Projectile>().connect<&ProjectileSystem::onProjectileRemoved>(this);
}

void drft::system::ProjectileSystem::update(float dt)
{
	auto view = registry->view<component::Position, component::Projectile, component::tag::CurrentActor>();
	for (auto [entity, pos, proj] : view.each())
	{
		if (proj.progress >= proj.line.size())
		{
			registry->remove<component::Projectile>(entity);
			continue;
		}

		auto delta = proj.line.at(proj.progress++) - pos.position;
		registry->emplace_or_replace<component::action::Move>(entity, delta);
	}
}

void drft::system::ProjectileSystem::onProjectileAdded(entt::registry& registry, entt::entity entity)
{
	auto& projectile = registry.get<component::Projectile>(entity);
	registry.emplace<component::Actor>(entity, 0, projectile.speed);

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
