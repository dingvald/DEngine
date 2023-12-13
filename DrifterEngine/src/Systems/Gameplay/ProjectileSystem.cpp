#include "pch.h"
#include "ProjectileSystem.h"
#include "Components/Components.h"
#include "Components/Tags.h"

void drft::system::ProjectileSystem::init()
{
	registry->on_construct<component::Projectile>().connect<&ProjectileSystem::onProjectileAdded>(this);
}

void drft::system::ProjectileSystem::update(float dt)
{
	auto view = registry->view<component::Position, component::Projectile, component::tag::CurrentActor>();
	for (auto [entity, pos, proj] : view.each())
	{
		if (proj.progress >= proj.line.size())
		{
			registry->remove<component::Projectile>(entity);
			registry->remove<component::Actor>(entity);
			continue;
		}

		auto delta = proj.line.at(proj.progress++) - pos.position;
		registry->emplace<component::action::Move>(entity, delta);
	}
}

void drft::system::ProjectileSystem::onProjectileAdded(entt::registry& registry, entt::entity entity)
{
	auto& projectile = registry.get<component::Projectile>(entity);
	registry.emplace<component::Actor>(entity, 100, projectile.speed);
}
