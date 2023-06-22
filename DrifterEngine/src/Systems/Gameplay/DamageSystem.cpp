#include <pch.h>
#include "DamageSystem.h"
#include "Components/Components.h"
#include "Utility/EntityHelpers.h"
#include "Systems/Helpers/GetExperienceFromKilling.h"

void drft::system::DamageSystem::init()
{
}

void drft::system::DamageSystem::update(const float dt)
{
	// This sepration of incoming / taking damage allows for event handlers to react to the events separately
	auto incomingDamageView = registry->view<component::action::IncomingDamage>();
	for (auto [entity, incoming] : incomingDamageView.each())
	{
		registry->emplace<component::action::TakeDamage>(entity, incoming.amount, incoming.source);
	}

	auto damageView = registry->view<component::action::TakeDamage, component::Health>();
	for (auto [entity, damage, health] : damageView.each())
	{
		health.current = std::clamp(health.current - damage.amount, 0, health.max);
		if (health.current == 0)
		{
			registry->emplace<component::action::Die>(entity);
			registry->emplace_or_replace<component::action::GainExperience>(damage.source, getExperienceFromKilling(entity, *registry));
		}
	}
}

void drft::system::DamageSystem::onUpdateEnd()
{
	registry->clear<component::action::IncomingDamage>();
	registry->clear<component::action::TakeDamage>();
}
