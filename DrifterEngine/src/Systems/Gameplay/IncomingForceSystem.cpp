#include "pch.h"
#include "IncomingForceSystem.h"
#include <Components/IncomingForceComponent.h>
#include <Components/Components.h>

namespace Internal
{
	static float convertForceToEffectiveDamage(float force)
	{
		return drft::math::floorToMultiple(force, 0.5f);
	}
}

void drft::system::IncomingForceSystem::init()
{
	_random.setSeed(rng::GlobalSeed);
}

void drft::system::IncomingForceSystem::update(const float dt)
{
	auto incomingForceView = _registry.view<IncomingForceComponent>();
	for (auto&& [entity, incomingForce] : incomingForceView.each())
	{
		entt::handle hit = { _registry, entity };
		entt::handle other = { _registry, incomingForce.entity };

		const float damageEffective = Internal::convertForceToEffectiveDamage(incomingForce.force);
		
		hit.emplace_or_replace<component::action::TakeDamage>(damageEffective, other.entity());
	}

	_registry.clear<IncomingForceComponent>();
}

