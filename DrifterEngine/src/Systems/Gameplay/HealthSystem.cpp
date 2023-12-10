#include <pch.h>
#include "HealthSystem.h"
#include "Components/Components.h"
#include "Utility/EntityHelpers.h"
#include "Systems/Helpers/GetExperienceFromKilling.h"
#include "Events/SendFloatingMessageEvent.h"
#include "Systems/Helpers/SpawnEffect.h"
#include "Systems/Helpers/GetPrimaryMaterial.h"

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
			auto handle = entt::const_handle{ *registry, entity };
			auto& physical = registry->get<component::Physical>(entity);
			
			auto material = getPrimaryMaterial(handle);
			sf::Color materialColor = material.get<component::Render>().color;
			std::string message;
			sf::Color messageColor = sf::Color::White;
			sf::Color effectColor = materialColor;
			std::vector<unsigned int> effectSprites = { handle.get<component::Render>().sprite };
			int effect_ttl = 10;

			if (damage.amount == 0)
			{
				messageColor = sf::Color::Blue;
				effectColor = sf::Color(180, 180, 180);
				effectSprites = { 18u };
				effect_ttl = 30;
			}
			else if (damage.amount < 0)
			{
				message += "+";
				messageColor = sf::Color::Green;
				effectColor = sf::Color::Green;
			}
			else if (damage.amount > 0)
			{
				// Spawn Hit particles
				spawnEffect(*registry, {
					.color = materialColor,
					.sprites = {80, 81, 82},
					.layer = RenderLayer::EffectsBack,
					.position = posComp->position,
					.animationSpeed = 12.0f
					});
			}

			auto& dispatcher = registry->ctx().get<entt::dispatcher&>();
			dispatcher.trigger(events::SendFloatingMessageEvent{
				.message = message + std::to_string(std::abs(damage.amount)),
				.color = messageColor,
				.position = posComp->position,
				.velocity = {0,-1},
				.fades = true,
				.isScreenSpace = false,
				.ttl = 80
				});

			// Spawn HurtEffect
			spawnEffect(*registry, {
			.color = effectColor,
			.sprites = effectSprites,
			.layer = RenderLayer::EffectsBack,
			.position = posComp->position,
			.animationSpeed = 10.0f,
			.ttl = effect_ttl,
			.fades = false,
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
