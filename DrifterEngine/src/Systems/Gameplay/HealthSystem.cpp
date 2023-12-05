#include <pch.h>
#include "HealthSystem.h"
#include "Components/Components.h"
#include "Utility/EntityHelpers.h"
#include "Systems/Helpers/GetExperienceFromKilling.h"
#include "Events/SendFloatingMessageEvent.h"

void drft::system::HealthSystem::init()
{
	auto& dispatcher = registry->ctx().get<entt::dispatcher&>();
	dispatcher.sink<events::TurnStartEvent>().connect<&HealthSystem::onTurnStartEvent>(this);
	registry->on_update<component::action::LevelUp>().connect<&HealthSystem::onLevelUp>(this);
}

void drft::system::HealthSystem::update(const float dt)
{
	// This sepration of incoming / taking damage allows for event handlers to react to the events separately
	auto incomingDamageView = registry->view<component::action::IncomingDamage>();
	for (auto [entity, incoming] : incomingDamageView.each())
	{
		int total = 0;
		for (auto& [_, damage] : incoming.damageTypes)
		{
			total += damage;
		}
		registry->emplace<component::action::TakeDamage>(entity, total, incoming.source);
	}

	auto damageView = registry->view<component::action::TakeDamage, component::Health>();
	for (auto [entity, damage, health] : damageView.each())
	{
		// send floating message
		if (auto posComp = registry->try_get<component::Position>(entity))
		{
			std::string message;
			sf::Color messageColor = sf::Color::White;
			if (damage.amount == 0)
			{
				messageColor = sf::Color::Blue;
			}
			else if (damage.amount < 0)
			{
				message += "+";
				messageColor = sf::Color::Green;
			}
			auto& dispatcher = registry->ctx().get<entt::dispatcher&>();
			dispatcher.trigger(events::SendFloatingMessageEvent{
				.message = message + std::to_string(std::abs(damage.amount)),
				.color = messageColor,
				.position = registry->get<component::Position>(entity).position,
				.velocity = {0,-1},
				.fades = true,
				.isScreenSpace = false,
				.ttl = 80
				});
		}
		
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
		const auto& levelUp = registry.get<component::action::LevelUp>(entity);
		if (levelUp.statChanges.contains("vitality"))
		{
			health->max += levelUp.statChanges.at("vitality");
		}
		health->current = health->max;
	}
}
