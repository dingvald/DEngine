#include <pch.h>
#include "HealthSystem.h"
#include "Components/Components.h"
#include "Utility/EntityHelpers.h"
#include "Systems/Helpers/GetExperienceFromKilling.h"

void drft::system::HealthSystem::init()
{
	auto& dispatcher = registry->ctx().get<entt::dispatcher&>();
	dispatcher.sink<events::TurnStartEvent>().connect<&HealthSystem::onTurnStartEvent>(this);
	registry->on_update<component::BaseStats>().connect<&HealthSystem::onLevelUp>(this);
}

void drft::system::HealthSystem::update(const float dt)
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
		health.current = std::clamp(health.current - damage.amount, 0.f, health.max);
		if (health.current == 0)
		{
			registry->emplace<component::action::Die>(entity);
			registry->emplace_or_replace<component::action::GainExperience>(damage.source, getExperienceFromKilling(entity, *registry));
		}
	}
}

void drft::system::HealthSystem::onUpdateEnd()
{
	registry->clear<component::action::IncomingDamage>();
	registry->clear<component::action::TakeDamage>();
}

void drft::system::HealthSystem::onTurnStartEvent(events::TurnStartEvent& ev)
{
	if (auto health = registry->try_get<component::Health>(ev.entity))
	{
		health->current = std::clamp(health->current + health->recovery, 1.f, health->max);
	}
}

void drft::system::HealthSystem::onLevelUp(entt::registry& registry, entt::entity entity)
{
	if (auto health = registry.try_get<component::Health>(entity))
	{
		health->current = health->max;
	}
}
